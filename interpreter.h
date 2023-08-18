#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"

typedef struct SymTable_t {
    Span id;
    Value value;
    struct SymTable* next;
} SymTable;

typedef struct Interpreter_t {
    Statement* root;
    SymTable* symbols;
} Interpreter;

Interpreter interpreter_init(Statement* root);
void interpreter_deinit(Interpreter* interpreter);
void interpreter_begin(Interpreter* interpreter);

#endif /* INTERPRETER_H */
