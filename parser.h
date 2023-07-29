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

VarDecl parse_vardecl(Parser* parser);

#endif /* PARSER_H */
