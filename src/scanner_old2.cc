#include <tree_sitter/parser.h>
#include <cassert>
#include <cstring>
#include <vector>

namespace
{
    using std::vector;

    const char HEREDOC_START = '<';
    const char HEREDOC_END = '>';
    const char BRACE_START = '{';
    const char TILDE = '~';
    const char DOLLAR = '$';
    const char ESCAPE = '\\';

    enum TokenType
    {
        COMMAND_START,
        COMMAND_CONTENT,
        COMMAND_END,
        // Not used in the grammar, but used in the external scanner to check for error state.
        // This relies on the tree-sitter behavior that when an error is encountered the external
        // scanner is called with all symobls marked as valid.
        ERROR,
    };

    struct Scanner
    {
        enum
        {
            CommandHeredoc = 1 << 0,
        };

        Scanner() : flags(0)
        {
            deserialize(NULL, 0);
        }

        unsigned serialize(char *buffer)
        {
            memcpy(&buffer[0], &flags, 1);
            return 1;
        }

        void deserialize(const char *buffer, unsigned length)
        {
            flags = 0;
            if (length > 0)
            {
                memcpy(&flags, &buffer[0], 1);
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
            else
                return false;
            advance(lexer);
            lexer->mark_end(lexer);
            lexer->result_symbol = COMMAND_START;
            return true;
        }

        bool scan_command_content(TSLexer *lexer)
        {
            assert(flags & CommandHeredoc);
            bool has_content = false;
            while (lexer->lookahead)
            {
                if (lexer->lookahead == TILDE)
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
                else if (lexer->lookahead == HEREDOC_END)
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
                if (valid_symbols[COMMAND_CONTENT])
                    return scan_command_content(lexer);
            }

            lexer->mark_end(lexer);
            skip_whitespace(lexer);
            if (valid_symbols[COMMAND_START])
                return scan_command_start(lexer);

            return false;
        }

        // the current parser state - we can be in a command, a string, or a string nested within
        // a placeholder within a command
        char flags;
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