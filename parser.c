#include "parser.h"

Parser parser_init(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current = lexer_gettok(parser.lexer);

    return parser;
}
