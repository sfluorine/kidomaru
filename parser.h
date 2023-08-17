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

Statement* parse_statement(Parser* parser);

#endif /* PARSER_H */
