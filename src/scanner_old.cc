#include <tree_sitter/parser.h>
#include <cassert>
#include <cstring>
#include <vector>

namespace
{
    using std::vector;

    const char SINGLE_QUOTE = '\'';
    const char DOUBLE_QUOTE = '"';
    const char HEREDOC_START = '<';
    const char HEREDOC_END = '>';
    const char BRACE_START = '{';
    const char BRACE_END = '}';
    const char TILDE = '~';
    const char DOLLAR = '$';
    const char ESCAPE = '\\';

    enum TokenType
    {
        STRING_START,
        STRING_CONTENT,
        STRING_END,
        COMMAND_START,
        COMMAND_CONTENT,
        COMMAND_END,
        // Not used in the grammar, but used in the external scanner to check for error state.
        // This relies on the tree-sitter behavior that when an error is encountered the external
        // scanner is called with all symobls marked as valid.
        ERROR,
    };

    struct StringDelim
    {
        enum
        {
            SingleQuote = 1 << 0,
            DoubleQuote = 1 << 1,
        };

        StringDelim() : flags(0) {}

        int32_t end_character() const
        {
            if (flags & SingleQuote)
                return SINGLE_QUOTE;
            if (flags & DoubleQuote)
                return DOUBLE_QUOTE;
            return 0;
        }

        void set_end_character(int32_t character)
        {
            switch (character)
            {
            case SINGLE_QUOTE:
                flags |= SingleQuote;
                break;
            case DOUBLE_QUOTE:
                flags |= DoubleQuote;
                break;
            default:
                assert(false);
            }
        }

        char flags;
    };

    struct Scanner
    {
        enum
        {
            String = 1 << 0,
            CommandHeredoc = 1 << 1,
            CommandBrace = 1 << 2,
        };

        Scanner() : flags(0)
        {
            assert(sizeof(StringDelim) == sizeof(char));
            deserialize(NULL, 0);
        }

        unsigned serialize(char *buffer)
        {
            size_t i = 0;
            memcpy(&buffer[i++], &flags, 1);
            size_t string_count = string_stack.size();
            if (string_count > UINT8_MAX)
                string_count = UINT8_MAX;
            if (string_count > 0)
            {
                memcpy(&buffer[i], string_stack.data(), string_count);
                i += string_count;
            }
            return i;
        }

        void deserialize(const char *buffer, unsigned length)
        {
            flags = 0;
            string_stack.clear();
            if (length > 0)
            {
                memcpy(&flags, &buffer[0], 1);
                if (length > 1)
                {
                    size_t string_count = length - 1;
                    string_stack.resize(string_count);
                    memcpy(string_stack.data(), &buffer[1], string_count);
                    assert(string_stack.size() == string_count);
                }
            }
        }

        void advance(TSLexer *lexer)
        {
            lexer->advance(lexer, false);
        }

        void skip(TSLexer *lexer)
        {
            lexer->advance(lexer, true);
        }

        bool scan_string_start(TSLexer *lexer)
        {
            char symbol = lexer->lookahead;
            if (symbol == SINGLE_QUOTE || symbol == DOUBLE_QUOTE)
            {
                StringDelim string;
                string.set_end_character(symbol);
                flags |= String;
                advance(lexer);
                lexer->mark_end(lexer);
                lexer->result_symbol = STRING_START;
                string_stack.push_back(string);
                return true;
            }
            return false;
        }

        bool scan_command_start(TSLexer *lexer)
        {
            assert(flags == 0);
            if (lexer->lookahead == HEREDOC_START)
            {
                advance(lexer);
                if (lexer->lookahead != HEREDOC_START)
                    return false;
                advance(lexer);
                if (lexer->lookahead != HEREDOC_START)
                    return false;
                flags |= CommandHeredoc;
            }
            else if (lexer->lookahead == BRACE_START)
                flags |= CommandBrace;
            else
                return false;
            advance(lexer);
            lexer->mark_end(lexer);
            lexer->result_symbol = COMMAND_START;
            return true;
        }

        bool scan_string_content(TSLexer *lexer)
        {
            char end_char = string_stack.back().end_character();
            bool has_content = false;
            while (lexer->lookahead)
            {
                if (lexer->lookahead == TILDE || lexer->lookahead == DOLLAR)
                {
                    lexer->mark_end(lexer);
                    advance(lexer);
                    if (lexer->lookahead == BRACE_START)
                    {
                        if (has_content)
                        {
                            lexer->result_symbol = STRING_CONTENT;
                            return true;
                        }
                        else
                            return false;
                    }
                    else
                        has_content = true;
                }
                else if (lexer->lookahead == ESCAPE)
                {
                    lexer->mark_end(lexer);
                    if (has_content)
                    {
                        lexer->result_symbol = STRING_CONTENT;
                        return true;
                    }
                    else
                        return false;
                }
                else if (lexer->lookahead == end_char)
                {
                    if (has_content)
                        lexer->result_symbol = STRING_CONTENT;
                    else
                    {
                        advance(lexer);
                        lexer->result_symbol = STRING_END;
                        string_stack.pop_back();
                        if (string_stack.empty())
                            flags ^= String;
                    }
                    lexer->mark_end(lexer);
                    return true;
                }
                // TODO: handle newline as error
                else
                {
                    advance(lexer);
                    has_content = true;
                }
            }
            // TODO: eof without reaching end of string should be an error
            return false;
        }

        bool scan_command_content(TSLexer *lexer)
        {
            char end_char;
            if (flags & CommandHeredoc)
                end_char = HEREDOC_END;
            else if (flags & CommandBrace)
                end_char = BRACE_END;
            else
                assert(false);
            bool has_content = false;
            while (lexer->lookahead)
            {
                // `${}` is only allowed within `command {}`
                if (lexer->lookahead == TILDE || (lexer->lookahead == DOLLAR && end_char == BRACE_END))
                {
                    lexer->mark_end(lexer);
                    advance(lexer);
                    if (lexer->lookahead == BRACE_START)
                    {
                        if (has_content)
                        {
                            lexer->result_symbol = COMMAND_CONTENT;
                            return true;
                        }
                        else
                            return false;
                    }
                    else
                        has_content = true;
                }
                else if (lexer->lookahead == ESCAPE)
                {
                    lexer->mark_end(lexer);
                    advance(lexer);
                    // we only recognize certain escape sequences within command blocks - if it's
                    // not one of the recognized characters then treat it as content
                    char symbol = lexer->lookahead;
                    if (symbol == HEREDOC_END ||
                        symbol == TILDE ||
                        (symbol == DOLLAR && end_char == BRACE_END) ||
                        symbol == ESCAPE)
                    {
                        if (has_content)
                        {
                            lexer->result_symbol = COMMAND_CONTENT;
                            return true;
                        }
                        else
                            return false;
                    }
                    else
                        has_content = true;
                }
                else if (lexer->lookahead == end_char)
                {
                    if (end_char == HEREDOC_END)
                    {
                        // we're in `command <<<` and the next character is `>` - look for
                        // two more
                        lexer->mark_end(lexer);
                        advance(lexer);
                        if (lexer->lookahead == HEREDOC_END)
                        {
                            advance(lexer);
                            if (lexer->lookahead == HEREDOC_END)
                            {
                                if (has_content)
                                    lexer->result_symbol = COMMAND_CONTENT;
                                else
                                {
                                    advance(lexer);
                                    lexer->mark_end(lexer);
                                    lexer->result_symbol = COMMAND_END;
                                    flags = 0;
                                }
                                return true;
                            }
                        }
                        has_content = true;
                    }
                    else
                    {
                        if (has_content)
                            lexer->result_symbol = COMMAND_CONTENT;
                        else
                        {
                            advance(lexer);
                            lexer->result_symbol = COMMAND_END;
                            flags = 0;
                        }
                        lexer->mark_end(lexer);
                        return true;
                    }
                }
                else
                {
                    advance(lexer);
                    has_content = true;
                }
            }
            return false;
        }

        void skip_whitespace(TSLexer *lexer)
        {
            while (lexer->lookahead == ' ' || lexer->lookahead == '\t')
                skip(lexer);
        }

        bool scan(TSLexer *lexer, const bool *valid_symbols)
        {
            bool is_error = valid_symbols[ERROR];
            if (flags > 0 && !is_error)
            {
                if (valid_symbols[STRING_CONTENT] && !string_stack.empty())
                    return scan_string_content(lexer);
                else if (valid_symbols[COMMAND_CONTENT])
                    return scan_command_content(lexer);
            }

            lexer->mark_end(lexer);
            skip_whitespace(lexer);
            if (valid_symbols[STRING_START])
                return scan_string_start(lexer);
            else if (valid_symbols[COMMAND_START])
                return scan_command_start(lexer);

            return false;
        }

        // the current parser state - we can be in a command, a string, or a string nested within
        // a placeholder within a command
        char flags;
        // use a stack to keep track of nested strings, e.g. "foo ~{"bar"}"
        vector<StringDelim> string_stack;
    };
}

extern "C"
{
    void *tree_sitter_wdl_external_scanner_create()
    {
        return new Scanner();
    }

    bool tree_sitter_wdl_external_scanner_scan(void *payload, TSLexer *lexer,
                                               const bool *valid_symbols)
    {
        Scanner *scanner = static_cast<Scanner *>(payload);
        return scanner->scan(lexer, valid_symbols);
    }

    unsigned tree_sitter_wdl_external_scanner_serialize(void *payload, char *buffer)
    {
        Scanner *scanner = static_cast<Scanner *>(payload);
        return scanner->serialize(buffer);
    }

    void tree_sitter_wdl_external_scanner_deserialize(void *payload, const char *buffer, unsigned length)
    {
        Scanner *scanner = static_cast<Scanner *>(payload);
        scanner->deserialize(buffer, length);
    }

    void tree_sitter_wdl_external_scanner_destroy(void *payload)
    {
        Scanner *scanner = static_cast<Scanner *>(payload);
        delete scanner;
    }
}