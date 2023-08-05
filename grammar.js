module.exports = grammar({
    name: "wdl",
    rules: {
        document: $ => seq(  // enforces that version comes first
            $.version,
            repeat1(  // enforces that there are one-or-more items
                choice(  // selects between multiple variants
                    $.import,
                    $.struct,
                    $.workflow,
                    $.task
                )
            )
        ),
        version: $ => seq("version", "1.1"),
        import: $ => todo("import"),
        struct: $ => todo("struct"),
        workflow: $ => todo("workflow"),
        task: $ => todo("task"),
    }
})

function todo(rule) {
    console.warn(`Missing implementation of rule ${rule}`);
    return rule
}