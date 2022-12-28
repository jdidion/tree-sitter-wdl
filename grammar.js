// language name
const LANGUAGE = "wdl"

// language keywords
const KEYWORD = {
    // types
    boolean: "Boolean",
    int: "Int",
    float: "Float",
    string: "String",
    file: "File",
    array: "Array",
    map: "Map",
    pair: "Pair",
    object: "Object",

    // expressions 
    if: "if",
    then: "then",
    else: "else",
    object_literal: "object",

    // top-level elements
    version: "version",
    import: "import",
    as: "as",
    alias: "alias",
    struct: "struct",
    workflow: "workflow",
    task: "task",
    input: "input",
    output: "output",
    call: "call",
    scatter: "scatter",
    in: "in",
    command: "command",
    runtime: "runtime",
    meta: "meta",
    parameter_meta: "parameter_meta",

    // literals
    true: "true",
    false: "false",
    none: "None",
    null: "null"
}

// operators
const OPER = {
    add: "+",
    sub: "-",
    mul: "*",
    div: "/",
    mod: "%",
    pos: "+",
    neg: "-",
    and: "&&",
    or: "||",
    not: "!",
    lt: "<",
    lte: "<=",
    gt: ">",
    gte: ">=",
    eq: "==",
    ne: "!=",
    assign: "=",
    optional: "?",
    non_empty: "+"
}

const SYMBOL = {
    lbrace: "{",
    rbrace: "}",
    lbrack: "[",
    rbrack: "]",
    lparen: "(",
    rparen: ")",
    colon: ":",
    comma: ",",
    dot: ".",
    squote: "'",
    dquote: '"',
    escape: "\\",
    tilde_placeholder: "~{",
    dollar_placeholder: "${",
    heredoc_start: "<<<",
    heredoc_end: ">>>",
    comment: "#"
}

// precedences
const PREC = {
    number: 12,
    group: 11,
    member: 10,
    index: 9,
    apply: 8,
    unary: 7,
    mul: 6,
    add: 5,
    less: 4,
    equal: 3,
    and: 2,
    or: 1
}

module.exports = grammar({
    name: LANGUAGE,

    extras: $ => [
        /\s/,
        $.comment
    ],

    supertypes: $ => [
        $.expression,
        $.primary_expression
    ],

    externals: $ => [
        $._string_start,
        $.string_content,
        $._string_end,
        $._command_start,
        $.command_content,
        $._command_end,
        // Not used in the grammar, but used in the external scanner to check for error state.
        // This relies on the tree-sitter behavior that when an error is encountered the external
        // scanner is called with all symobls marked as valid.
        $._error,
    ],

    word: $ => $.identifier,

    rules: {
        document: $ => seq(
            $.version,
            repeat(
                choice(
                    $.import,
                    $.struct,
                    $.workflow,
                    $.task
                )
            )
        ),

        version: $ => seq(
            KEYWORD.version,
            field("identifier", choice(
                $.float,
                $.simple_string
            ))
        ),

        import: $ => seq(
            KEYWORD.import,
            field("uri", $.simple_string),
            optional(
                seq(
                    KEYWORD.as,
                    field("namespace", $.identifier)
                )
            ),
            optional(field("aliases", $.import_aliases))
        ),

        import_aliases: $ => repeat1($.import_alias),

        import_alias: $ => seq(
            KEYWORD.alias,
            field("from", $.identifier),
            KEYWORD.as,
            field("to", $.identifier)
        ),

        struct: $ => seq(
            KEYWORD.struct,
            field("name", $.identifier),
            field("fields", $.struct_fields)
        ),

        struct_fields: $ => block(
            repeat1(alias($.unbound_declaration, $.struct_field)),
        ),

        workflow: $ => seq(
            KEYWORD.workflow,
            field("name", $.identifier),
            field("body", $.workflow_body),
        ),

        workflow_body: $ => block(
            repeat1(
                choice(
                    $.input,
                    $.output,
                    $.bound_declaration,
                    $.call,
                    $.scatter,
                    $.conditional,
                    $.meta,
                    $.parameter_meta
                )
            )
        ),

        call: $ => seq(
            KEYWORD.call,
            field("target", $.qualified_identifier),
            optional(
                seq(
                    KEYWORD.as,
                    field("alias", $.identifier)
                )
            ),
            optional(field("inputs", $.call_inputs))
        ),

        call_inputs: $ => block(
            seq(
                KEYWORD.input,
                SYMBOL.colon,
                commaSep($.call_input)
            )
        ),

        call_input: $ => seq(
            field("name", $.identifier),
            optional(
                seq(
                    OPER.assign,
                    field("expression", $.expression)
                )
            )
        ),

        scatter: $ => seq(
            KEYWORD.scatter,
            SYMBOL.lparen,
            field("name", $.identifier),
            KEYWORD.in,
            field("expression", $.expression),
            SYMBOL.rparen,
            field("body", alias($.workflow_block, $.scatter_body)),
        ),

        conditional: $ => seq(
            KEYWORD.if,
            SYMBOL.lparen,
            field("expression", $.expression),
            SYMBOL.rparen,
            field("body", alias($.workflow_block, $.conditional_body)),
        ),

        workflow_block: $ => block(
            repeat1(
                choice(
                    $.bound_declaration,
                    $.call,
                    $.scatter,
                    $.conditional
                )
            )
        ),

        task: $ => seq(
            KEYWORD.task,
            field("name", $.identifier),
            field("body", $.task_elements),
        ),

        task_elements: $ => choice(
            seq(SYMBOL.lbrace, SYMBOL.rbrace),
            seq(
                SYMBOL.lbrace,
                repeat1(
                    choice(
                        $.input,
                        $.output,
                        $.bound_declaration,
                        $.command,
                        $.runtime,
                        $.meta,
                        $.parameter_meta
                    )
                ),
                SYMBOL.rbrace
            )
        ),

        command: $ => seq(
            KEYWORD.command,
            alias($._command_start, SYMBOL.heredoc_start),
            optional(field("parts", $.command_parts)),
            alias($._command_end, SYMBOL.heredoc_end)
        ),

        command_parts: $ => repeat1(
            choice(
                $.placeholder,
                alias($.command_escape_sequence, $.escape_sequence),
                $.command_content,
            )
        ),

        command_escape_sequence: $ => token(prec(1, seq(
            SYMBOL.escape,
            /[>~$\\]/
        ))),

        runtime: $ => seq(
            KEYWORD.runtime,
            field("attributes", $.runtime_attributes)
        ),

        runtime_attributes: $ => block(repeat1($.runtime_attribute)),

        runtime_attribute: $ => seq(
            field("name", $.identifier),
            SYMBOL.colon,
            field("expression", $.expression)
        ),

        input: $ => seq(
            KEYWORD.input,
            field("declarations", $.input_declarations)
        ),

        input_declarations: $ => block(
            repeat1(
                choice(
                    $.unbound_declaration,
                    $.bound_declaration
                )
            )
        ),

        output: $ => seq(
            KEYWORD.output,
            field("declarations", $.output_declarations)
        ),

        output_declarations: $ => block(repeat1($.bound_declaration)),

        unbound_declaration: $ => seq(
            field("type", $._type),
            field("name", $.identifier),
        ),

        bound_declaration: $ => seq(
            field("type", $._type),
            field("name", $.identifier),
            OPER.assign,
            field("expression", $.expression)
        ),

        _type: $ => prec.right(choice(
            $.optional_type,
            $._required_type,
        )),

        optional_type: $ => seq(
            field("type", $._required_type),
            OPER.optional
        ),

        _required_type: $ => choice(
            $._simple_type,
            $._array_type,
            $.map_type,
            $.pair_type,
            $.user_type
        ),

        _simple_type: $ => prec(1, choice(
            $.boolean_type,
            $.int_type,
            $.float_type,
            $.string_type,
            $.file_type,
            $.object_type
        )),

        boolean_type: $ => KEYWORD.boolean,
        int_type: $ => KEYWORD.int,
        float_type: $ => KEYWORD.float,
        string_type: $ => KEYWORD.string,
        file_type: $ => KEYWORD.file,
        object_type: $ => KEYWORD.object,

        user_type: $ => prec(0, field("name", $.identifier)),

        _array_type: $ => prec.right(choice(
            $.nonempty_array_type,
            alias($._maybe_empty_array_type, $.array_type)
        )),

        nonempty_array_type: $ => seq(
            $._maybe_empty_array_type,
            OPER.non_empty,
        ),

        _maybe_empty_array_type: $ => seq(
            KEYWORD.array,
            SYMBOL.lbrack,
            field("type", $._type),
            SYMBOL.rbrack,
        ),

        map_type: $ => seq(
            KEYWORD.map,
            SYMBOL.lbrack,
            field("key", $._type),
            ",",
            field("value", $._type),
            SYMBOL.rbrack
        ),

        pair_type: $ => seq(
            KEYWORD.pair,
            SYMBOL.lbrack,
            field("left", $._type),
            ",",
            field("right", $._type),
            SYMBOL.rbrack
        ),

        expression: $ => choice(
            $.primary_expression,
            $.and_operator,
            $.or_operator,
            $.not_operator,
            $.comparison_operator
        ),

        primary_expression: $ => choice(
            $.unary_operator,
            $.binary_operator,
            $.apply_expression,
            $.index_expression,
            $.field_expression,
            $.ternary_expression,
            $.group_expression,
            $._literal,
            $.identifier
        ),

        unary_operator: $ => prec(PREC.unary, seq(
            field("operator", choice(OPER.pos, OPER.neg)),
            field("expression", $.primary_expression)
        )),

        binary_operator: $ => {
            const table = [
                [OPER.add, PREC.add],
                [OPER.sub, PREC.add],
                [OPER.mul, PREC.mul],
                [OPER.div, PREC.mul],
                [OPER.mod, PREC.mul]
            ];

            return choice(...table.map(([operator, precedence]) => prec.left(precedence, seq(
                field("left", $.primary_expression),
                field("operator", operator),
                field("right", $.primary_expression)
            ))));
        },

        apply_expression: $ => prec(PREC.apply, seq(
            field("name", $.identifier),
            field("arguments", $.argument_list)
        )),

        argument_list: $ => choice(
            seq(SYMBOL.lparen, SYMBOL.rparen),
            seq(
                SYMBOL.lparen,
                commaSep1($.expression),
                SYMBOL.rparen
            )
        ),

        index_expression: $ => prec(PREC.index, seq(
            field("collection", $.primary_expression),
            SYMBOL.lbrack,
            field("index", $.primary_expression),
            SYMBOL.rbrack
        )),

        field_expression: $ => prec(PREC.member, seq(
            field("object", $.primary_expression),
            SYMBOL.dot,
            field("name", $.identifier)
        )),

        ternary_expression: $ => prec.right(seq(
            KEYWORD.if,
            field("condition", $.expression),
            KEYWORD.then,
            field("true", $.expression),
            KEYWORD.else,
            field("false", $.expression)
        )),

        group_expression: $ => seq(
            SYMBOL.lparen,
            field("expression", $.expression),
            SYMBOL.rparen
        ),

        _literal: $ => choice(
            $._primitive,
            $.array,
            $.map,
            $.pair,
            $.object,
            $.none
        ),

        _primitive: $ => choice(
            $._bool,
            $._number,
            $.string
        ),

        _bool: $ => choice($.true, $.false),
        true: $ => KEYWORD.true,
        false: $ => KEYWORD.false,
        none: $ => KEYWORD.none,

        _number: $ => choice(
            $.hex_int,
            $.oct_int,
            $.dec_int,
            $.float
        ),

        hex_int: $ => token(seq(choice('0x', '0X'), /[\da-fA-F]+/)),

        oct_int: $ => token(seq(choice('0o', '0O'), /[0-7]+/)),

        dec_int: $ => prec.right(PREC.number, token(/[\+-]?\d+/)),

        float: $ => {
            const sign = /[\+-]?/
            const digits = /\d+/
            const exponent = seq(/[eE][\+-]?/, digits)

            return prec.right(PREC.number, token(choice(
                seq(sign, digits, '.', optional(digits), optional(exponent)),
                seq(sign, optional(digits), '.', digits, optional(exponent)),
                seq(sign, digits, exponent)
            )))
        },

        string: $ => seq(
            alias($._string_start, SYMBOL.dquote),
            optional(field("parts", $.string_parts)),
            alias($._string_end, SYMBOL.dquote)
        ),

        string_parts: $ => repeat1(
            choice(
                $.placeholder,
                $.escape_sequence,
                $.string_content
            )
        ),

        simple_string: $ => choice(
            seq(
                SYMBOL.squote,
                optional(field("parts", alias($.simple_string_squote_parts, $.string_parts))),
                SYMBOL.squote
            ),
            seq(
                SYMBOL.dquote,
                optional(field("parts", alias($.simple_string_dquote_parts, $.string_parts))),
                SYMBOL.dquote
            ),
        ),

        simple_string_squote_parts: $ => repeat1(
            choice(
                $.escape_sequence,
                alias(/[^'\\\n]+/, $.string_content)
            )
        ),

        simple_string_dquote_parts: $ => repeat1(
            choice(
                $.escape_sequence,
                alias(/[^"\\\n]+/, $.string_content)
            )
        ),

        placeholder: $ => choice(
            $._tilde_placeholder,
            $._dollar_placeholder
        ),

        _tilde_placeholder: $ => seq(
            SYMBOL.tilde_placeholder,
            field("expression", $.expression),
            SYMBOL.rbrace
        ),

        _dollar_placeholder: $ => seq(
            SYMBOL.dollar_placeholder,
            field("expression", $.expression),
            SYMBOL.rbrace
        ),

        escape_sequence: $ => token(prec(1, seq(
            SYMBOL.escape,
            choice(
                /u[a-fA-F\d]{4}/,
                /U[a-fA-F\d]{8}/,
                /x[a-fA-F\d]{2}/,
                /[0-7]{3}/,
                /['"nt~$\\]/
            )
        ))),

        array: $ => field("elements", $.array_elements),

        array_elements: $ => choice(
            seq(SYMBOL.lbrack, SYMBOL.rbrack),
            seq(
                SYMBOL.lbrack,
                commaSep1($.expression),
                SYMBOL.rbrack
            )
        ),

        map: $ => field("entries", $.map_entries),

        map_entries: $ => choice(
            seq(SYMBOL.lbrace, SYMBOL.rbrace),
            seq(
                SYMBOL.lbrace,
                commaSep1($.map_entry),
                SYMBOL.rbrace
            )
        ),

        map_entry: $ => seq(
            field("key", $.expression),
            SYMBOL.colon,
            field("value", $.expression)
        ),

        pair: $ => seq(
            SYMBOL.lparen,
            field("left", $.expression),
            SYMBOL.comma,
            field("right", $.expression),
            SYMBOL.rparen
        ),

        object: $ => seq(
            field("type", choice(
                alias(KEYWORD.object_literal, KEYWORD.object),
                $.identifier
            )),
            field("fields", $.object_fields)
        ),

        object_fields: $ => choice(
            seq(SYMBOL.lbrace, SYMBOL.rbrace),
            seq(
                SYMBOL.lbrace,
                commaSep1($.object_field),
                SYMBOL.rbrace
            )
        ),

        object_field: $ => seq(
            field("name", $.identifier),
            SYMBOL.colon,
            field("expression", $.expression)
        ),

        and_operator: $ => prec.left(PREC.and, seq(
            field("left", $.expression),
            field("operator", OPER.and),
            field("right", $.expression)
        )),

        or_operator: $ => prec.left(PREC.or, seq(
            field("left", $.expression),
            field("operator", OPER.or),
            field("right", $.expression)
        )),

        not_operator: $ => prec(PREC.unary, seq(
            field("operator", OPER.not),
            field("expression", $.expression)
        )),

        comparison_operator: $ => {
            const table = [
                [OPER.lt, PREC.less],
                [OPER.lte, PREC.less],
                [OPER.gt, PREC.less],
                [OPER.gte, PREC.less],
                [OPER.eq, PREC.equal],
                [OPER.ne, PREC.equal],
            ];

            return choice(...table.map(([operator, precedence]) => prec.left(precedence, seq(
                field("left", $.primary_expression),
                field("operator", operator),
                field("right", $.primary_expression)
            ))));
        },

        qualified_identifier: $ => seq(
            $.identifier,
            repeat(
                seq(
                    SYMBOL.dot,
                    $.identifier
                )
            )
        ),

        identifier: $ => /[a-zA-Z][a-zA-Z0-9_]*/,

        meta: $ => seq(
            KEYWORD.meta,
            field("attributes", $.meta_attributes)
        ),

        parameter_meta: $ => seq(
            KEYWORD.parameter_meta,
            field("attributes", $.meta_attributes)
        ),

        meta_attributes: $ => block(repeat1($.meta_attribute)),

        meta_attribute: $ => seq(
            field("name", $.identifier),
            SYMBOL.colon,
            field("value", $._meta_value)
        ),

        _meta_value: $ => choice(
            $.null,
            $._bool,
            $._number,
            $.simple_string,
            $.meta_array,
            $.meta_object
        ),

        null: $ => KEYWORD.null,

        meta_array: $ => field("elements", $.meta_array_elements),

        meta_array_elements: $ => choice(
            seq(SYMBOL.lbrack, SYMBOL.rbrack),
            seq(
                SYMBOL.lbrack,
                commaSep1($._meta_value),
                SYMBOL.rbrack
            )
        ),

        meta_object: $ => field("fields", $.meta_object_fields),

        meta_object_fields: $ => block(commaSep1($.meta_object_field)),

        meta_object_field: $ => seq(
            field("name", $.identifier),
            SYMBOL.colon,
            field("value", $._meta_value)
        ),

        // TODO: how to ensure a full-line comment is not nested under another node
        comment: $ => token(seq(SYMBOL.comment, /.*/))
    }
});

function commaSep(rule) {
    return optional(commaSep1(rule))
}

function commaSep1(rule) {
    return seq(
        rule,
        repeat(seq(",", rule)),
        optional(",")
    )
}

function block(rule) {
    return choice(
        seq(SYMBOL.lbrace, SYMBOL.rbrace),
        seq(
            SYMBOL.lbrace,
            rule,
            SYMBOL.rbrace
        )
    )
}