#include <stdlib.h>

#include "ast.h"

void value_deinit(Value* value) {
    if (value->kind != VAL_STRING)
        return;

    free(value->String.data);
}

void expr_deinit(Expr* expr) {
    if (expr->kind == EXPR_PRIMARY) {
        value_deinit(&expr->Primary);
        free(expr);
    } else {
        expr_deinit(expr->Binary.lhs); 
        expr_deinit(expr->Binary.rhs);

        free(expr);
    }
}

void vardecl_deinit(VarDecl* vardecl) {
    expr_deinit(vardecl->expr);
}

void ast_deinit(Ast* ast) {
    switch (ast->kind) {

    case AST_VAR_DECL:
        vardecl_deinit(&ast->vardecl);
        break;
    }

    free(ast);
}
