#ifndef AST_H
#define AST_H

#include <stdint.h>

#include "lexer.h"

typedef enum ValueKind_t {
    VAL_INT,
    VAL_DOUBLE,
    VAL_BOOL,
    VAL_STRING,
    VAL_IDENT,
} ValueKind;

typedef struct Value_t {
    ValueKind kind;

    union {
        int64_t i64;
        double f64;
        int bool;
        struct {
            char* data;
            size_t size;
        } String;
        Span span;
    };
} Value;

void value_deinit(Value* value);

typedef enum ExprKind_t {
    EXPR_BINARY,
    EXPR_PRIMARY,
} ExprKind;

typedef struct Expr_t Expr;

struct Expr_t {
    ExprKind kind;

    union {
        struct {
            Expr* lhs;
            Expr* rhs;
            char op;
        } Binary;

        Value Primary;
    };
};

void expr_deinit(Expr* expr);

typedef struct VarDecl_t {
    Span id;
    ValueKind type;
    Expr* expr;
} VarDecl;

void var_decl_deinit(VarDecl* vardecl);

struct BlockStatement_t;

typedef struct IfStatement_t {
    Expr* expr;
    struct BlockStatement_t* if_block; 
    struct BlockStatement_t* else_block; 
} IfStatement;

void if_statement_deinit(IfStatement* ifstatement);

typedef enum StatementKind_t {
    STATEMENT_VAR_DECL,
    STATEMENT_IF_STATEMENT,
    STATEMENT_BLOCK_STATEMENT,
    STATEMENT_RETURN,
} StatementKind;

typedef struct Statement_t {
    StatementKind kind;

    union {
        VarDecl vardecl;
        IfStatement ifstatement;
        struct BlockStatement_t* blockstatement;
        Expr* ret;
    };
} Statement;

void statement_deinit(Statement* statement);

typedef struct BlockStatement_t {
    Statement* statement;
    struct BlockStatement_t* next;
} BlockStatement;

void block_statement_deinit(BlockStatement* blockstatement);

#endif /* AST_H */
