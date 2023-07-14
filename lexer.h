#ifndef LEXER_H
#define LEXER_H

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
} TokenKind;

typedef struct Token_t {
    TokenKind kind;
    Span span;
} Token;

Lexer lexer_init(const char* input);

/* this function will init a span from a null terminated string. */
Span span_from(const char* data);
Span span_init(const char* data, size_t size);

int span_equals(Span lhs, Span rhs);

#endif /* LEXER_H */
