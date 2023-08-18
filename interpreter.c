#include <stdio.h>
#include <stdlib.h>

#include "interpreter.h"

static void evaluate_statement(Interpreter* interpreter, Statement* statement);
static void evaluate_var_decl(Interpreter* interpreter, VarDecl* vardecl);
static void evaluate_if_statement(Interpreter* interpreter, IfStatement* ifstatement);
static void evaluate_block_statement(Interpreter* interpreter, BlockStatement* blockstatement);

static Value evaluate_expression(Expr* expr);
static int64_t evaluate_binop_int(int64_t lhs, int64_t rhs, char op);
static double evaluate_binop_double(double lhs, double rhs, char op);

Interpreter interpreter_init(Statement* root) {
    return (Interpreter) {
        .root = root,
        .symbols = NULL,
    };
}

void interpreter_deinit(Interpreter* interpreter) {
    statement_deinit(interpreter->root);

    if (interpreter->symbols != NULL) {
        value_deinit(&interpreter->symbols->value);
        free(interpreter->symbols);
    }
}

void interpreter_begin(Interpreter* interpreter) {
    evaluate_statement(interpreter, interpreter->root);
}

static void evaluate_statement(Interpreter* interpreter, Statement* statement) {
    switch (interpreter->root->kind) {
    case STATEMENT_VAR_DECL:
        evaluate_var_decl(interpreter, &statement->vardecl);
        break;
    case STATEMENT_IF:
        evaluate_if_statement(interpreter, &statement->ifstatement);
        break;
    case STATEMENT_BLOCK:
        evaluate_block_statement(interpreter, statement->blockstatement);
        break;
    default:
        fprintf(stderr, "ERROR: unsupported statement\n");
        exit(1);
    }
}

static void evaluate_var_decl(Interpreter* interpreter, VarDecl* vardecl) {
    SymTable* new_node = malloc(sizeof(SymTable));
    if (new_node == NULL) {
        fprintf(stderr, "ERROR: cannot allocate memory!\n");
        exit(1);
    }

    Value expr_value = evaluate_expression(vardecl->expr);

    if (vardecl->type != expr_value.kind) {
        fprintf(stderr, "ERROR: mismatch types for variable declaration\n");
        fprintf(stderr, "    lhs: %d\n", vardecl->type);
        fprintf(stderr, "    rhs: %d\n", expr_value.kind);
        exit(1);
    }

    new_node->id = vardecl->id;
    new_node->value = expr_value;


    new_node->next = interpreter->symbols;
    interpreter->symbols = new_node;
}

static void evaluate_if_statement(Interpreter* interpreter, IfStatement* ifstatement) {
    Value bool_val = evaluate_expression(ifstatement->expr);

    if (bool_val.kind != VAL_BOOL) {
        fprintf(stderr, "ERROR: expected boolean expression but got else\n");
        exit(1);
    }

    if (bool_val.bool) {
        evaluate_block_statement(interpreter, ifstatement->if_block);
    } else {
        if (ifstatement->else_block == NULL)
            return;

        evaluate_block_statement(interpreter, ifstatement->else_block);
    }
}

static void evaluate_block_statement(Interpreter* interpreter, BlockStatement* blockstatement) {
    while (1) {
        evaluate_statement(interpreter, blockstatement->statement);
        blockstatement = blockstatement->next;
    }
}

static Value evaluate_expression(Expr* expr) {
    if (expr->kind == EXPR_BINARY) {
        Value lhs = evaluate_expression(expr->Binary.lhs);
        Value rhs = evaluate_expression(expr->Binary.rhs);

        if (lhs.kind != rhs.kind) {
            fprintf(stderr, "ERROR: invalid operands for binary operator '%c'\n", expr->Binary.op);
            fprintf(stderr, "    lhs: %d\n", lhs.kind);
            fprintf(stderr, "    rhs: %d\n", rhs.kind);
            exit(1);
        }

        switch (lhs.kind) {
        case VAL_INT:
            return (Value) {
                .kind = lhs.kind,
                .i64  = evaluate_binop_int(lhs.i64, rhs.i64, expr->Binary.op),
            };
        case VAL_DOUBLE:
            return (Value) {
                .kind = lhs.kind,
                .f64  = evaluate_binop_double(lhs.f64, rhs.f64, expr->Binary.op),
            };
        default:
            exit(1); // unreachable
        }
    }

    return expr->Primary;
}

static int64_t evaluate_binop_int(int64_t lhs, int64_t rhs, char op) {
    switch (op) {
    case '+':
        return lhs + rhs;
    case '-':
        return lhs - rhs;
    case '*':
        return lhs * rhs;
    case '/':
        return lhs / rhs;
    default:
        fprintf(stderr, "ERROR: invalid binary operation!\n");
        exit(1);
    }
}

static double evaluate_binop_double(double lhs, double rhs, char op) {
    switch (op) {
    case '+':
        return lhs + rhs;
    case '-':
        return lhs - rhs;
    case '*':
        return lhs * rhs;
    case '/':
        return lhs / rhs;
    default:
        fprintf(stderr, "ERROR: invalid binary operation!\n");
        exit(1);
    }
}
