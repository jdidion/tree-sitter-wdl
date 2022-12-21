# tree-sitter-wdl

[Tree-sitter](https://github.com/tree-sitter/tree-sitter) grammar for [WDL](https://openwdl.org/) (Workflow Description Language).

## Design

This repository provides a single [grammar](grammar.js) that parses WDL versions `1.x` (`draft-*` and `development` versions are *not* supported). The grammar is designed to be permissive and error-tolerant. A parser generated from this grammar will allow token combinations that are forbidden by the WDL specification or that are only allowed in certain WDL versions.

As Tree-sitter generates LR(1) parsers, only one token of look-ahead is available and thus syntax elements that require multiple tokens of look-ahead are difficult or impossible to write. For WDL, this means strings (including the contents of the `command` block) cannot be described by the grammar. Fortunately, Tree-sitter allows for external parsing of these syntax elements. The external parser for the WDL grammar is in [scanner.cc](src/tree_sitter/../scanner.cc) and implements the API described in the [Tree-sitter documentation](https://tree-sitter.github.io/tree-sitter/creating-parsers#external-scanners).

## Development

* Follow the instructions to install the [Tree-sitter CLI](https://github.com/tree-sitter/tree-sitter/blob/master/cli/README.md)
* Generate and compile the parser: `npm run build`
* Run the tests `npm run test`