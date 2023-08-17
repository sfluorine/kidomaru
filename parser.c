#include <stdio.h>
#include <string.h>
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
    "IF",
    "ELSE",

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

static Expr* parse_primary(Parser* parser);
static Expr* parse_expression(Parser* parser, TokenKind delim, size_t prec);

static VarDecl parse_var_decl(Parser* parser);
static IfStatement parse_if_statement(Parser* parser);
static BlockStatement* parse_block_statement(Parser* parser);

static ValueKind parse_type(Parser* parser);

Parser parser_init(Lexer* lexer) {
    return (Parser) {
        .lexer = lexer,
        .current = lexer_gettok(lexer),
    };
}

Statement* parse_statement(Parser* parser) {
    Statement* statement = malloc(sizeof(Statement));

    if (statement == NULL) {
        fprintf(stderr, "ERROR: cannot allocate memory!\n");
        exit(1);
    }

    if (parser->current.kind == TOK_LET) {
        statement->kind = STATEMENT_VAR_DECL;
        statement->vardecl = parse_var_decl(parser);

        return statement;
    } 

    if (parser->current.kind == TOK_IF) {
        statement->kind = STATEMENT_IF_STATEMENT;
        statement->ifstatement = parse_if_statement(parser);

        return statement;
    }

    if (parser->current.kind == TOK_RETURN) {
        advance(parser);

        statement->kind = STATEMENT_RETURN;
        statement->ret = parse_expression(parser, TOK_SEMICOLON, 0);

        match(parser, TOK_SEMICOLON);

        return statement;
    }

    fprintf(stderr, "(%zu:%zu) ERROR: expected statement but got %s!\n", parser->current.line, parser->current.col, token_stringified[parser->current.kind]);
    exit(1);
}

static int is_eof(Parser* parser) {
    return parser->current.kind == TOK_EOF;
}

static int expect(Parser* parser, TokenKind kind) {
    return parser->current.kind == kind;
}

static size_t get_prec(Token token) {
    switch (token.kind) {
    case TOK_IDENTIFIER:
    case TOK_INTLITERAL:
    case TOK_DOUBLELITERAL:
    case TOK_BOOLTRUE:
    case TOK_BOOLFALSE:
    case TOK_STRINGLITERAL:
        return 0;
    case TOK_PLUS:
    case TOK_MINUS:
        return 1;
    case TOK_STAR:
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

static Expr* parse_primary(Parser* parser) {
    Expr* expr = malloc(sizeof(Expr));

    if (expr == NULL) {
        fprintf(stderr, "ERROR: cannot allocate memory!\n");
        exit(1);
    }

    switch (parser->current.kind) {
    case TOK_INTLITERAL:
        expr->kind = EXPR_PRIMARY;
        expr->Primary = (Value) {
            .kind = VAL_INT,
            .i64  = strtol(parser->current.span.data, NULL, 10),
        };

        break;
    case TOK_DOUBLELITERAL:
        expr->kind = EXPR_PRIMARY;
        expr->Primary = (Value) {
            .kind = VAL_DOUBLE,
            .f64  = strtod(parser->current.span.data, NULL),
        };

        break;
    case TOK_BOOLTRUE:
        expr->kind = EXPR_PRIMARY;
        expr->Primary = (Value) {
            .kind = VAL_BOOL,
            .bool = 1,
        };

        break;
    case TOK_BOOLFALSE:
        expr->kind = EXPR_PRIMARY;
        expr->Primary = (Value) {
            .kind = VAL_BOOL,
            .bool = 0,
        };

        break;
    case TOK_STRINGLITERAL:
        expr->kind = EXPR_PRIMARY;
        expr->Primary = (Value) {
            .kind   = VAL_STRING,
            .String = {
                .data = strndup(parser->current.span.data, parser->current.span.size),
                .size = parser->current.span.size,
            },
        };

        break;
    case TOK_IDENTIFIER:
        expr->kind = EXPR_PRIMARY;
        expr->Primary = (Value) {
            .kind = VAL_IDENT,
            .span = parser->current.span,
        };

        break;
    default:
        fprintf(stderr, "(%zu:%zu) ERROR: expected value but got %s\n", parser->current.line, parser->current.col, token_stringified[parser->current.kind]);
        exit(1);
    }

    advance(parser);

    return expr;
}

static Expr* parse_expression(Parser* parser, TokenKind delim, size_t prec) {
    Expr* left = parse_primary(parser);

    Token curr_tok = parser->current;

    if (curr_tok.kind == delim)
        return left;

    size_t new_prec = get_prec(curr_tok);

    if (new_prec == 0) {
        fprintf(stderr, "(%zu:%zu) ERROR: expected a binary operator but got %s\n", curr_tok.line, curr_tok.col, token_stringified[curr_tok.kind]);
        exit(1);
    }

    while (new_prec >= prec) {
        Expr* binop = malloc(sizeof(Expr));

        binop->kind = EXPR_BINARY;

        switch (curr_tok.kind) {
        case TOK_PLUS:
            binop->Binary.op = '+';
            break;
        case TOK_MINUS:
            binop->Binary.op = '-';
            break;
        case TOK_STAR:
            binop->Binary.op = '*';
            break;
        case TOK_SLASH:
            binop->Binary.op = '/';
            break;
        default:
            fprintf(stderr, "(%zu:%zu) ERROR: unreachable!\n", curr_tok.line, curr_tok.col);
            exit(1);
        }

        advance(parser);

        binop->Binary.lhs = left;
        binop->Binary.rhs = parse_expression(parser, delim, new_prec);

        left = binop;

        if (parser->current.kind == delim)
            break;
    }

    return left;
}

static VarDecl parse_var_decl(Parser* parser) {
    VarDecl vardecl;

    match(parser, TOK_LET);

    Span id = parser->current.span;

    match(parser, TOK_IDENTIFIER);

    vardecl.id = id;

    match(parser, TOK_COLON);

    ValueKind type = parse_type(parser);
    vardecl.type = type;

    match(parser, TOK_EQUAL);

    Expr* expr = parse_expression(parser, TOK_SEMICOLON, 0);
    vardecl.expr = expr;

    match(parser, TOK_SEMICOLON);

    return vardecl;
}

static IfStatement parse_if_statement(Parser* parser) {
    IfStatement ifstatement;

    match(parser, TOK_IF);

    match(parser, TOK_LPAREN);
    Expr* expr = parse_expression(parser, TOK_RPAREN, 0);
    match(parser, TOK_RPAREN);

    ifstatement.expr = expr;
    ifstatement.if_block = parse_block_statement(parser);

    if (parser->current.kind == TOK_ELSE) {
        advance(parser);

        ifstatement.else_block = parse_block_statement(parser);
        return ifstatement;
    }

    ifstatement.else_block = NULL;
    return ifstatement;
}

static BlockStatement* parse_block_statement(Parser* parser) {
    BlockStatement* blockstatement = malloc(sizeof(BlockStatement));
    if (blockstatement == NULL) {
        fprintf(stderr, "ERROR: cannot allocate memory!\n");
        exit(1);
    }

    blockstatement->next = NULL;

    match(parser, TOK_LBRACE);
    blockstatement->statement = parse_statement(parser);
    match(parser, TOK_RBRACE);


    return blockstatement;
}

static ValueKind parse_type(Parser* parser) {
    Token token = parser->current;

    switch (token.kind) {
    case TOK_TYPEI64:
        advance(parser);
        return VAL_INT;
    case TOK_TYPEF64:
        advance(parser);
        return VAL_DOUBLE;
    case TOK_TYPEBOOL:
        advance(parser);
        return VAL_BOOL;
    case TOK_TYPESTRING:
        advance(parser);
        return VAL_STRING;
    default:
        fprintf(stderr, "(%zu:%zu) ERROR: expected type but got %s\n", token.line, token.col, token_stringified[token.kind]);
        exit(1);
    }
}
