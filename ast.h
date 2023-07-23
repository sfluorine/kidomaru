#ifndef AST_H
#define AST_H

#include <stdint.h>

typedef enum ValueKind_t {
    VAL_INT,
    VAL_DOUBLE,
    VAL_BOOL,
    VAL_STRING,
} ValueKind;

typedef struct Value_t {
    ValueKind kind;

    union {
        int64_t i64;
        double f64;
        int bool;
        char* string;
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

#endif /* AST_H */
