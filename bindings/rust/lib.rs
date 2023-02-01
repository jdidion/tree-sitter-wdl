//! This crate provides wdl language support for the [tree-sitter][] parsing library.
//!
//! Typically, you will use the [language][language func] function to add this language to a
//! tree-sitter [Parser][], and then use the parser to parse some code:
//!
//! ```
//! let code = "";
//! let mut parser = tree_sitter::Parser::new();
//! parser.set_language(tree_sitter_wdl::language()).expect("Error loading wdl grammar");
//! let tree = parser.parse(code, None).unwrap();
//! ```
//!
//! [Language]: https://docs.rs/tree-sitter/*/tree_sitter/struct.Language.html
//! [language func]: fn.language.html
//! [Parser]: https://docs.rs/tree-sitter/*/tree_sitter/struct.Parser.html
//! [tree-sitter]: https://tree-sitter.github.io/
use tree_sitter::{Language, LanguageError, Parser};

extern "C" {
    fn tree_sitter_wdl() -> Language;
}

/// The content of the [`node-types.json`][] file for this grammar.
///
/// [`node-types.json`]: https://tree-sitter.github.io/tree-sitter/using-parsers#static-node-types
pub const NODE_TYPES: &'static str = include_str!("../../src/node-types.json");

/// Returns the tree-sitter [Language][] for this grammar.
///
/// [Language]: https://docs.rs/tree-sitter/*/tree_sitter/struct.Language.html
pub fn language() -> Language {
    unsafe { tree_sitter_wdl() }
}

/// Returns a `Parser` with the language set to `language()`.
pub fn parser() -> Result<Parser, LanguageError> {
    let mut parser = Parser::new();
    parser.set_language(language())?;
    Ok(parser)
}

#[cfg(test)]
mod tests {
    use super::parser;
    
    #[test]
    fn test_can_load_grammar() {
        parser().expect("Error loading wdl language");
    }
}
