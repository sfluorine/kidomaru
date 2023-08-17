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

void if_statement_deinit(IfStatement* ifstatement) {
    expr_deinit(ifstatement->expr);
    block_statement_deinit(ifstatement->if_block);

    if (ifstatement->else_block != NULL)
        block_statement_deinit(ifstatement->else_block);
}

void statement_deinit(Statement* statement) {
    if (statement == NULL)
        return;

    switch (statement->kind) {
    case STATEMENT_VAR_DECL:
        vardecl_deinit(&statement->vardecl);
        break;
    case STATEMENT_IF_STATEMENT:
        if_statement_deinit(&statement->ifstatement);
        break;
    case STATEMENT_BLOCK_STATEMENT:
        block_statement_deinit(statement->blockstatement);
        break;
    case STATEMENT_RETURN:
        expr_deinit(statement->ret);
        break;
    }

    free(statement);
}

void block_statement_deinit(BlockStatement* blockstatement) {
    if (blockstatement == NULL)
        return;

    block_statement_deinit(blockstatement->next);
    statement_deinit(blockstatement->statement);
    free(blockstatement);
}
