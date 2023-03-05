use criterion::{criterion_group, criterion_main, Criterion};
use std::{fs, path::PathBuf};
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

criterion_group!(benches, benchmark_parser);
criterion_main!(benches);
