#include <stdlib.h>

#include "ast.h"

void value_deinit(Value* value) {
    if (value->kind != VALUE_STRING)
        return;

    free(value->string);
}

void expr_deinit(Expr* expr) {
    if (expr->kind == EXPR_PRIMARY) {
        value_deinit(&expr->Primary);
        return;
    }

    expr_deinit(expr->Binary.lhs);
    expr_deinit(expr->Binary.rhs);
}
