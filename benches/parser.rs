use criterion::{criterion_group, criterion_main, Criterion};
use std::{fs, path::PathBuf};
use tree_sitter::TreeCursor;
use tree_sitter_wdl_1;

pub fn benchmark_parser(c: &mut Criterion) {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let test_dir = root.join("resources").join("test");
    let mut group = c.benchmark_group("parser");

    let comprehensive_path = test_dir.join("strings.wdl");
    let comprehensive_text = fs::read_to_string(comprehensive_path.clone()).unwrap();
    group.bench_function("strings", |b| {
        b.iter(|| tree_sitter_wdl_1::parse_document(&comprehensive_text))
    });

    let comprehensive_path = test_dir.join("expressions.wdl");
    let comprehensive_text = fs::read_to_string(comprehensive_path.clone()).unwrap();
    group.bench_function("expressions", |b| {
        b.iter(|| tree_sitter_wdl_1::parse_document(&comprehensive_text))
    });

    let comprehensive_path = test_dir.join("comprehensive.wdl");
    let comprehensive_text = fs::read_to_string(comprehensive_path.clone()).unwrap();
    group.bench_function("comprehensive", |b| {
        b.iter(|| tree_sitter_wdl_1::parse_document(&comprehensive_text))
    });
}

fn exhaust_recursive<'a>(tree: &mut TreeCursor<'a>, ancestors: &mut Vec<usize>) {
    let node = tree.node();
    if tree.goto_first_child() {
        ancestors.push(node.id());
        exhaust_recursive(tree, ancestors);
        while tree.goto_next_sibling() {
            exhaust_recursive(tree, ancestors);
        }
        tree.goto_parent();
        assert_eq!(tree.node().id(), ancestors.pop().unwrap());
    }
}

enum State {
    Pending(usize),
    Iterating,
}

fn exhaust_iterative<'a>(tree: &mut TreeCursor<'a>) {
    let mut ancestors = Vec::new();
    let mut state = State::Pending(tree.node().id());
    loop {
        match state {
            State::Pending(parent_id) if tree.goto_first_child() => {
                ancestors.push(parent_id);
                state = State::Pending(tree.node().id());
            }
            State::Pending(_) => state = State::Iterating,
            State::Iterating if tree.goto_next_sibling() => {
                state = State::Pending(tree.node().id())
            }
            State::Iterating => {
                tree.goto_parent();
                assert_eq!(tree.node().id(), ancestors.pop().unwrap());
                if ancestors.is_empty() {
                    break;
                }
            }
        }
    }
}

pub fn benchmark_parser_exhaust(c: &mut Criterion) {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let test_dir = root.join("resources").join("test");
    let comprehensive_path = test_dir.join("comprehensive.wdl");
    let mut group = c.benchmark_group("parser_exhaust");
    let comprehensive_text = fs::read_to_string(comprehensive_path.clone()).unwrap();
    group.bench_function("exhaust_recursive", |b| {
        b.iter(|| {
            let doc = tree_sitter_wdl_1::parse_document(&comprehensive_text).unwrap();
            let mut ancestors = Vec::new();
            exhaust_recursive(&mut doc.walk(), &mut ancestors);
        })
    });
    group.bench_function("exhaust_iterative", |b| {
        b.iter(|| {
            let doc = tree_sitter_wdl_1::parse_document(&comprehensive_text).unwrap();
            exhaust_iterative(&mut doc.walk());
        })
    });
}

criterion_group!(benches, benchmark_parser, benchmark_parser_exhaust);
criterion_main!(benches);
