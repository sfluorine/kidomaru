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

static size_t get_prec(Token token);

static void advance(Parser* parser);
static void match(Parser* parser, TokenKind kind);

static void parse_primary(Parser* parser);

Parser parser_init(Lexer* lexer) {
    return (Parser) {
        .lexer = lexer,
        .current = lexer_gettok(lexer),
    };
}

void parse_expression(Parser* parser, TokenKind delim, size_t prec) {
    parse_primary(parser);

    if (expect(parser, TOK_EOF) || expect(parser, delim))
        return;

    size_t new_prec = get_prec(parser->current);
    
    if (new_prec == 0) {
        fprintf(stderr, "(%zu:%zu) ERROR: expected binary operator but got %s\n", parser->current.line, parser->current.col, token_stringified[parser->current.kind]);
        exit(1);
    }

    while (new_prec >= prec) {
        advance(parser);

        parse_expression(parser, delim, new_prec);

        if (expect(parser, TOK_EOF) || expect(parser, delim))
            return;
    }
}

static int is_eof(Parser* parser) {
    return parser->current.kind == TOK_EOF;
}

static int expect(Parser* parser, TokenKind kind) {
    return parser->current.kind == kind;
}

static size_t get_prec(Token token) {
    switch (token.kind) {
    case TOK_INTLITERAL:
    case TOK_DOUBLELITERAL:
    case TOK_BOOLTRUE:
    case TOK_BOOLFALSE:
    case TOK_STRINGLITERAL:
        return 0;
    case TOK_PLUS:
        return 1;
    case TOK_MINUS:
        return 1;
    case TOK_STAR:
        return 2;
    case TOK_SLASH:
        return 2;
    default:
        fprintf(stderr, "(%zu:%zu) ERROR: cannot get precedence from an invalid token!\n", token.line, token.col);
        exit(1);
    }
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

static void parse_primary(Parser* parser) {
    switch (parser->current.kind) {
    case TOK_INTLITERAL:
    case TOK_DOUBLELITERAL:
    case TOK_BOOLTRUE:
    case TOK_BOOLFALSE:
    case TOK_STRINGLITERAL:
        advance(parser);
        break;
    default:
        fprintf(stderr, "(%zu:%zu) ERROR: expected value but got %s\n", parser->current.line, parser->current.col, token_stringified[parser->current.kind]);
        exit(1);
    }
}
