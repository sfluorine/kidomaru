#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef struct Parser_t {
    Lexer* lexer;
    Token current;
} Parser;

Parser parser_init(Lexer* lexer);

#endif /* PARSER_H */
