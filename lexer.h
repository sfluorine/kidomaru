#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stddef.h>

typedef struct Lexer_t {
    const char* input;
    size_t line;
    size_t col;
} Lexer;

typedef struct Span_t {
    const char* data;
    size_t size;
} Span;

typedef enum TokenKind_t {
    TOK_EOF,
    TOK_IDENTIFIER,

    TOK_TYPEI64,
    TOK_TYPEF64,
    TOK_TYPEBOOL,
    TOK_TYPESTRING,

    TOK_INTLITERAL,
    TOK_DOUBLELITERAL,
    TOK_BOOLTRUE,
    TOK_BOOLFALSE,
    TOK_STRINGLITERAL,

    TOK_FN,
    TOK_LET,
    TOK_RETURN,
    TOK_IF,
    TOK_ELSE,

    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,

    TOK_COLON,
    TOK_SEMICOLON,
    TOK_EQUAL,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_COMMA,
    TOK_ARROW,

    TOK_GARBAGE,
} TokenKind;

typedef struct Token_t {
    TokenKind kind;
    Span span;
    size_t line;
    size_t col;
} Token;

Lexer lexer_init(const char* input);

/* this function will init a span from a null terminated string. */
Span span_from(const char* data);
Span span_init(const char* data, size_t size);

int span_equals(Span lhs, Span rhs);
void span_print(FILE* file, Span span);

Token token_init(TokenKind kind, Span span, size_t line, size_t col);

Token lexer_gettok(Lexer* lexer);

#endif /* LEXER_H */
