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

use thiserror::Error;
use tree_sitter::Language;

extern "C" {
    fn tree_sitter_wdl() -> Language;
}

/// Get the tree-sitter [Language][] for this grammar.
///
/// [Language]: https://docs.rs/tree-sitter/*/tree_sitter/struct.Language.html
pub fn language() -> Language {
    unsafe { tree_sitter_wdl() }
}

/// The content of the [`node-types.json`][] file for this grammar.
///
/// [`node-types.json`]: https://tree-sitter.github.io/tree-sitter/using-parsers#static-node-types
pub const NODE_TYPES: &'static str = include_str!("../../src/node-types.json");

// Uncomment these to include any queries that this grammar contains

// pub const HIGHLIGHTS_QUERY: &'static str = include_str!("../../queries/highlights.scm");
// pub const INJECTIONS_QUERY: &'static str = include_str!("../../queries/injections.scm");
// pub const LOCALS_QUERY: &'static str = include_str!("../../queries/locals.scm");
// pub const TAGS_QUERY: &'static str = include_str!("../../queries/tags.scm");

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
    use std::path::PathBuf;

    #[test]
    fn test_can_load_grammar() {
        let mut parser = tree_sitter::Parser::new();
        parser
            .set_language(super::language())
            .expect("Error loading wdl language");
    }

    /// Test that a simple WDL file can be parsed.
    #[test]
    fn test_parse() {
        let wdl_file = PathBuf::from(env!("CARGO_MANIFEST_DIR"))
            .join("resources")
            .join("test")
            .join("interesting.wdl");
        let wdl_source = std::fs::read_to_string(wdl_file).unwrap();
        let tree = super::parse_document(&wdl_source).unwrap();
        let root = tree.root_node(); // get the root (`document`) node in the tree
        // now we can get child nodes by their field names
        let version = root
            .child_by_field_name("version")
            .expect("Expected version node");
        let body = root
            .child_by_field_name("body")
            .expect("Expected body node");
        let cursor = &mut body.walk();
        // if we use the cursor within a scope that returns an owned Node instance,
        // then we're free to reuse the cursor after the end of the scope to get
        // the Node's children
        let workflow_body = {
            let mut body_children = body.children(cursor);
            let workflow = body_children.next().expect("Expected workflow node");
            let workflow_name = workflow
                .child_by_field_name("name")
                .expect("Expected name node");
            assert_eq!(
                workflow_name.utf8_text(&wdl_source.as_bytes()).unwrap(),
                "test"
            );
            workflow
                .child_by_field_name("body")
                .expect("Expected body node")
        };
        // we can also collect child nodes into a Vec and reuse the cursor afterwards
        let body_children: Vec<tree_sitter::Node> = workflow_body.children(cursor).collect();
        assert_eq!(body_children.len(), 3);
        let k = body_children[2];
        assert_eq!(k.kind(), "declaration");
        let mut k_children = k.children(cursor);
        let k_type = k_children.next().expect("Expected type node");
        assert_eq!(k_type.kind(), "Int"); // there is no `_type` node since it's hidden
        let k_name = k_children.next().expect("Expected name node");
        assert_eq!(k_name.kind(), "identifier");
        assert_eq!(k_name.utf8_text(&wdl_source.as_bytes()).unwrap(), "k");
        assert_eq!(k_children.next().expect("Expected = node").kind(), "=");
        let k_expr = k_children.next().expect("Expected expression node");
        assert_eq!(k_expr.kind(), "addition"); // there is no `_expression` node
        let left = k_expr
            .child_by_field_name("left")
            .expect("Expected left operand node");
        assert_eq!(left.kind(), "identifier");
        assert_eq!(left.utf8_text(&wdl_source.as_bytes()).unwrap(), "i");
        assert_eq!(
            k_expr
                .child_by_field_name("operator")
                .expect("Expected operator node")
                .kind(),
            "+"
        );
        let right = k_expr
            .child_by_field_name("right")
            .expect("Expected right operand node");
        assert_eq!(right.kind(), "multiplication"); // again, no `_expression` node
        let left = right
            .child_by_field_name("left")
            .expect("Expected left operand node");
        assert_eq!(left.kind(), "identifier");
        assert_eq!(left.utf8_text(&wdl_source.as_bytes()).unwrap(), "j");
        assert_eq!(
            k_expr
                .child_by_field_name("operator")
                .expect("Expected operator node")
                .kind(),
            "*"
        );
        let right = k_expr
            .child_by_field_name("right")
            .expect("Expected right operand node");
        assert_eq!(right.kind(), "integer");
        assert_eq!(right.utf8_text(&wdl_source.as_bytes()).unwrap(), "3");
    }
}
