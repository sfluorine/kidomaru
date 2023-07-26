#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

#include "lexer.h"
#include "ast.h"

typedef struct Parser_t {
    Lexer* lexer;
    Token current;
} Parser;

Parser parser_init(Lexer* lexer);

void parse_expression(Parser* parser, TokenKind delim, size_t prec);

#endif /* PARSER_H */
