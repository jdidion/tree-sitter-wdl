//! This crate provides wdl language support for the [tree-sitter][] parsing library.
use thiserror::Error;
use tree_sitter::Language;

extern "C" {
    fn tree_sitter_wdl() -> Language;
}

/// The source of the WDL tree-sitter grammar description.
pub const GRAMMAR: &str = include_str!("../../grammar.js");

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

#[derive(Error, Debug)]
pub enum ParserError {
    #[error("Error creating parser for WDL 1.x")]
    Language { source: tree_sitter::LanguageError },
    #[error("WDL document is empty")]
    DocumentEmpty,
}

/// Returns a `Parser` with the language set to `language()`.
pub fn parser() -> Result<tree_sitter::Parser, ParserError> {
    let mut parser = tree_sitter::Parser::new();
    parser
        .set_language(language())
        .map_err(|source| ParserError::Language { source })?;
    Ok(parser)
}

pub fn parse_document(text: &str) -> Result<tree_sitter::Tree, ParserError> {
    let mut parser = parser()?;
    parser
        .parse(text, None)
        .ok_or_else(|| ParserError::DocumentEmpty)
}

#[cfg(test)]
mod tests {
    #[test]
    fn test_can_load_grammar() {
        super::parser().expect("Error loading wdl language");
    }
}
