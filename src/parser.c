#include <tree_sitter/parser.h>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

#define LANGUAGE_VERSION 14
#define STATE_COUNT 8
#define LARGE_STATE_COUNT 4
#define SYMBOL_COUNT 10
#define ALIAS_COUNT 0
#define TOKEN_COUNT 7
#define EXTERNAL_TOKEN_COUNT 0
#define FIELD_COUNT 0
#define MAX_ALIAS_SEQUENCE_LENGTH 2
#define PRODUCTION_ID_COUNT 1

enum {
  anon_sym_version = 1,
  anon_sym_1_DOT1 = 2,
  sym_import = 3,
  sym_struct = 4,
  sym_workflow = 5,
  sym_task = 6,
  sym_document = 7,
  sym_version = 8,
  aux_sym_document_repeat1 = 9,
};

static const char * const ts_symbol_names[] = {
  [ts_builtin_sym_end] = "end",
  [anon_sym_version] = "version",
  [anon_sym_1_DOT1] = "1.1",
  [sym_import] = "import",
  [sym_struct] = "struct",
  [sym_workflow] = "workflow",
  [sym_task] = "task",
  [sym_document] = "document",
  [sym_version] = "version",
  [aux_sym_document_repeat1] = "document_repeat1",
};

static const TSSymbol ts_symbol_map[] = {
  [ts_builtin_sym_end] = ts_builtin_sym_end,
  [anon_sym_version] = anon_sym_version,
  [anon_sym_1_DOT1] = anon_sym_1_DOT1,
  [sym_import] = sym_import,
  [sym_struct] = sym_struct,
  [sym_workflow] = sym_workflow,
  [sym_task] = sym_task,
  [sym_document] = sym_document,
  [sym_version] = sym_version,
  [aux_sym_document_repeat1] = aux_sym_document_repeat1,
};

static const TSSymbolMetadata ts_symbol_metadata[] = {
  [ts_builtin_sym_end] = {
    .visible = false,
    .named = true,
  },
  [anon_sym_version] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_1_DOT1] = {
    .visible = true,
    .named = false,
  },
  [sym_import] = {
    .visible = true,
    .named = true,
  },
  [sym_struct] = {
    .visible = true,
    .named = true,
  },
  [sym_workflow] = {
    .visible = true,
    .named = true,
  },
  [sym_task] = {
    .visible = true,
    .named = true,
  },
  [sym_document] = {
    .visible = true,
    .named = true,
  },
  [sym_version] = {
    .visible = true,
    .named = true,
  },
  [aux_sym_document_repeat1] = {
    .visible = false,
    .named = false,
  },
};

static const TSSymbol ts_alias_sequences[PRODUCTION_ID_COUNT][MAX_ALIAS_SEQUENCE_LENGTH] = {
  [0] = {0},
};

static const uint16_t ts_non_terminal_alias_map[] = {
  0,
};

static const TSStateId ts_primary_state_ids[STATE_COUNT] = {
  [0] = 0,
  [1] = 1,
  [2] = 2,
  [3] = 3,
  [4] = 4,
  [5] = 5,
  [6] = 6,
  [7] = 7,
};

static bool ts_lex(TSLexer *lexer, TSStateId state) {
  START_LEXER();
  eof = lexer->eof(lexer);
  switch (state) {
    case 0:
      if (eof) ADVANCE(29);
      if (lookahead == '1') ADVANCE(1);
      if (lookahead == 'i') ADVANCE(11);
      if (lookahead == 's') ADVANCE(24);
      if (lookahead == 't') ADVANCE(3);
      if (lookahead == 'v') ADVANCE(5);
      if (lookahead == 'w') ADVANCE(15);
      if (lookahead == '\t' ||
          lookahead == '\n' ||
          lookahead == '\r' ||
          lookahead == ' ') SKIP(0)
      END_STATE();
    case 1:
      if (lookahead == '.') ADVANCE(2);
      END_STATE();
    case 2:
      if (lookahead == '1') ADVANCE(31);
      END_STATE();
    case 3:
      if (lookahead == 'a') ADVANCE(22);
      END_STATE();
    case 4:
      if (lookahead == 'c') ADVANCE(26);
      END_STATE();
    case 5:
      if (lookahead == 'e') ADVANCE(20);
      END_STATE();
    case 6:
      if (lookahead == 'f') ADVANCE(10);
      END_STATE();
    case 7:
      if (lookahead == 'i') ADVANCE(13);
      END_STATE();
    case 8:
      if (lookahead == 'k') ADVANCE(35);
      END_STATE();
    case 9:
      if (lookahead == 'k') ADVANCE(6);
      END_STATE();
    case 10:
      if (lookahead == 'l') ADVANCE(14);
      END_STATE();
    case 11:
      if (lookahead == 'm') ADVANCE(17);
      END_STATE();
    case 12:
      if (lookahead == 'n') ADVANCE(30);
      END_STATE();
    case 13:
      if (lookahead == 'o') ADVANCE(12);
      END_STATE();
    case 14:
      if (lookahead == 'o') ADVANCE(28);
      END_STATE();
    case 15:
      if (lookahead == 'o') ADVANCE(21);
      END_STATE();
    case 16:
      if (lookahead == 'o') ADVANCE(19);
      END_STATE();
    case 17:
      if (lookahead == 'p') ADVANCE(16);
      END_STATE();
    case 18:
      if (lookahead == 'r') ADVANCE(27);
      END_STATE();
    case 19:
      if (lookahead == 'r') ADVANCE(25);
      END_STATE();
    case 20:
      if (lookahead == 'r') ADVANCE(23);
      END_STATE();
    case 21:
      if (lookahead == 'r') ADVANCE(9);
      END_STATE();
    case 22:
      if (lookahead == 's') ADVANCE(8);
      END_STATE();
    case 23:
      if (lookahead == 's') ADVANCE(7);
      END_STATE();
    case 24:
      if (lookahead == 't') ADVANCE(18);
      END_STATE();
    case 25:
      if (lookahead == 't') ADVANCE(32);
      END_STATE();
    case 26:
      if (lookahead == 't') ADVANCE(33);
      END_STATE();
    case 27:
      if (lookahead == 'u') ADVANCE(4);
      END_STATE();
    case 28:
      if (lookahead == 'w') ADVANCE(34);
      END_STATE();
    case 29:
      ACCEPT_TOKEN(ts_builtin_sym_end);
      END_STATE();
    case 30:
      ACCEPT_TOKEN(anon_sym_version);
      END_STATE();
    case 31:
      ACCEPT_TOKEN(anon_sym_1_DOT1);
      END_STATE();
    case 32:
      ACCEPT_TOKEN(sym_import);
      END_STATE();
    case 33:
      ACCEPT_TOKEN(sym_struct);
      END_STATE();
    case 34:
      ACCEPT_TOKEN(sym_workflow);
      END_STATE();
    case 35:
      ACCEPT_TOKEN(sym_task);
      END_STATE();
    default:
      return false;
  }
}

static const TSLexMode ts_lex_modes[STATE_COUNT] = {
  [0] = {.lex_state = 0},
  [1] = {.lex_state = 0},
  [2] = {.lex_state = 0},
  [3] = {.lex_state = 0},
  [4] = {.lex_state = 0},
  [5] = {.lex_state = 0},
  [6] = {.lex_state = 0},
  [7] = {.lex_state = 0},
};

static const uint16_t ts_parse_table[LARGE_STATE_COUNT][SYMBOL_COUNT] = {
  [0] = {
    [ts_builtin_sym_end] = ACTIONS(1),
    [anon_sym_version] = ACTIONS(1),
    [anon_sym_1_DOT1] = ACTIONS(1),
    [sym_import] = ACTIONS(1),
    [sym_struct] = ACTIONS(1),
    [sym_workflow] = ACTIONS(1),
    [sym_task] = ACTIONS(1),
  },
  [1] = {
    [sym_document] = STATE(7),
    [sym_version] = STATE(4),
    [anon_sym_version] = ACTIONS(3),
  },
  [2] = {
    [aux_sym_document_repeat1] = STATE(3),
    [ts_builtin_sym_end] = ACTIONS(5),
    [sym_import] = ACTIONS(7),
    [sym_struct] = ACTIONS(7),
    [sym_workflow] = ACTIONS(7),
    [sym_task] = ACTIONS(7),
  },
  [3] = {
    [aux_sym_document_repeat1] = STATE(3),
    [ts_builtin_sym_end] = ACTIONS(9),
    [sym_import] = ACTIONS(11),
    [sym_struct] = ACTIONS(11),
    [sym_workflow] = ACTIONS(11),
    [sym_task] = ACTIONS(11),
  },
};

static const uint16_t ts_small_parse_table[] = {
  [0] = 2,
    STATE(2), 1,
      aux_sym_document_repeat1,
    ACTIONS(14), 4,
      sym_import,
      sym_struct,
      sym_workflow,
      sym_task,
  [10] = 1,
    ACTIONS(16), 4,
      sym_import,
      sym_struct,
      sym_workflow,
      sym_task,
  [17] = 1,
    ACTIONS(18), 1,
      anon_sym_1_DOT1,
  [21] = 1,
    ACTIONS(20), 1,
      ts_builtin_sym_end,
};

static const uint32_t ts_small_parse_table_map[] = {
  [SMALL_STATE(4)] = 0,
  [SMALL_STATE(5)] = 10,
  [SMALL_STATE(6)] = 17,
  [SMALL_STATE(7)] = 21,
};

static const TSParseActionEntry ts_parse_actions[] = {
  [0] = {.entry = {.count = 0, .reusable = false}},
  [1] = {.entry = {.count = 1, .reusable = false}}, RECOVER(),
  [3] = {.entry = {.count = 1, .reusable = true}}, SHIFT(6),
  [5] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_document, 2),
  [7] = {.entry = {.count = 1, .reusable = true}}, SHIFT(3),
  [9] = {.entry = {.count = 1, .reusable = true}}, REDUCE(aux_sym_document_repeat1, 2),
  [11] = {.entry = {.count = 2, .reusable = true}}, REDUCE(aux_sym_document_repeat1, 2), SHIFT_REPEAT(3),
  [14] = {.entry = {.count = 1, .reusable = true}}, SHIFT(2),
  [16] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_version, 2),
  [18] = {.entry = {.count = 1, .reusable = true}}, SHIFT(5),
  [20] = {.entry = {.count = 1, .reusable = true}},  ACCEPT_INPUT(),
};

#ifdef __cplusplus
extern "C" {
#endif
#ifdef _WIN32
#define extern __declspec(dllexport)
#endif

extern const TSLanguage *tree_sitter_wdl(void) {
  static const TSLanguage language = {
    .version = LANGUAGE_VERSION,
    .symbol_count = SYMBOL_COUNT,
    .alias_count = ALIAS_COUNT,
    .token_count = TOKEN_COUNT,
    .external_token_count = EXTERNAL_TOKEN_COUNT,
    .state_count = STATE_COUNT,
    .large_state_count = LARGE_STATE_COUNT,
    .production_id_count = PRODUCTION_ID_COUNT,
    .field_count = FIELD_COUNT,
    .max_alias_sequence_length = MAX_ALIAS_SEQUENCE_LENGTH,
    .parse_table = &ts_parse_table[0][0],
    .small_parse_table = ts_small_parse_table,
    .small_parse_table_map = ts_small_parse_table_map,
    .parse_actions = ts_parse_actions,
    .symbol_names = ts_symbol_names,
    .symbol_metadata = ts_symbol_metadata,
    .public_symbol_map = ts_symbol_map,
    .alias_map = ts_non_terminal_alias_map,
    .alias_sequences = &ts_alias_sequences[0][0],
    .lex_modes = ts_lex_modes,
    .lex_fn = ts_lex,
    .primary_state_ids = ts_primary_state_ids,
  };
  return &language;
}
#ifdef __cplusplus
}
#endif
