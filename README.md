# tree-sitter-wdl

[Tree-sitter](https://github.com/tree-sitter/tree-sitter) grammar for [WDL](https://openwdl.org/) (Workflow Description Language).

## Design

This repository provides a single [grammar](grammar.js) that parses WDL versions `1.x` (`draft-*` and `development` versions are *not* supported). The grammar is designed to be permissive and error-tolerant. A parser generated from this grammar will allow token combinations that are forbidden by the WDL specification or that are only allowed in certain WDL versions.

## Development

* Follow the instructions to install the [Tree-sitter CLI](https://github.com/tree-sitter/tree-sitter/blob/master/cli/README.md)
* Generate and compile the parser: `npm run build`
* Run the tests `npm run test`