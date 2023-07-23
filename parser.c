#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

static const char* token_stringified[] = {
    "EOF",
    "IDENTIFIER",

    "TYPE I64",
    "TYPE F64",
    "TYPE BOOL",
    "TYPE STRING",

    "INT LITERAL",
    "DOUBLE LITERAL",
    "BOOL TRUE",
    "BOOL FALSE",
    "STRING LITERAL",

    "LET",
    "FN",
    "RETURN",

    "+",
    "-",
    "*",
    "/",

    ":",
    ";",
    "=",
    "(",
    ")",
    "{",
    "}",
    ",",
    "->",

    "GARBAGE",
};

static int is_eof(Parser* parser);
static int expect(Parser* parser, TokenKind kind);

static void advance(Parser* parser);
static void match(Parser* parser, TokenKind kind);

Parser parser_init(Lexer* lexer) {
    return (Parser) {
        .lexer = lexer,
        .current = lexer_gettok(lexer),
    };
}

void parse_expression(Parser* parser) {
    match(parser, TOK_EOF);
}

static int is_eof(Parser* parser) {
    return parser->current.kind == TOK_EOF;
}

static int expect(Parser* parser, TokenKind kind) {
    return parser->current.kind == kind;
}

static void advance(Parser* parser) {
    if (!is_eof(parser))
        parser->current = lexer_gettok(parser->lexer);
}

static void match(Parser* parser, TokenKind kind) {
    if (!expect(parser, kind)) {
        fprintf(stderr, "(%zu:%zu) ERROR: expected %s but got %s\n", parser->current.line, parser->current.col, token_stringified[kind], token_stringified[parser->current.kind]);
        exit(1);
    }

    advance(parser);
}
