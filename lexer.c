#include "lexer.h"

Lexer lexer_init(const char* input) {
    return (Lexer) {
        .input = input,
        .line = 1,
        .col = 1,
    };
}

Span span_from(const char* data) {
    size_t size = 0;
    for (; data[size] != 0; size++) {}

    return (Span) {
        .data = data,
        .size = size,
    };
}

Span span_init(const char* data, size_t size) {
    return (Span) {
        .data = data,
        .size = size,
    };
}

int span_equals(Span lhs, Span rhs) {
    if (lhs.size != rhs.size)
        return 0;

    for (size_t i = 0; i < lhs.size; i++) {
        if (lhs.data[i] != rhs.data[i])
            return 0;
    }

    return 1;
}
