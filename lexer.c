#include <ctype.h>

#include "lexer.h"

static char current(Lexer* lexer);
static char peek(Lexer* lexer);
static int is_eof(Lexer* lexer);

static void advance(Lexer* lexer);
static void skip_whitespaces(Lexer* lexer);

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

void span_print(FILE* file, Span span) {
    for (size_t i = 0; i < span.size; i++)
        fprintf(file, "%c", span.data[i]);
}

Token token_init(TokenKind kind, Span span, size_t line, size_t col) {
    return (Token) {
        .kind = kind,
        .span = span,
        .line = line,
        .col = col,
    };
}

Token lexer_gettok(Lexer* lexer) {
    skip_whitespaces(lexer);

    const char* curr_input = lexer->input;
    size_t curr_line = lexer->line;
    size_t curr_col  = lexer->col;

    if (is_eof(lexer))
        return token_init(TOK_EOF, span_from(""), curr_line, curr_col);

    switch (current(lexer)) {
    case '+':
        advance(lexer);
        return token_init(TOK_PLUS, span_from("+"), curr_line, curr_col);
    case '*':
        advance(lexer);
        return token_init(TOK_STAR, span_from("*"), curr_line, curr_col);
    case '/':
        advance(lexer);
        return token_init(TOK_SLASH, span_from("/"), curr_line, curr_col);
    case ':':
        advance(lexer);
        return token_init(TOK_COLON, span_from(":"), curr_line, curr_col);
    case ';':
        advance(lexer);
        return token_init(TOK_SEMICOLON, span_from(";"), curr_line, curr_col);
    case '=':
        advance(lexer);
        return token_init(TOK_EQUAL, span_from("="), curr_line, curr_col);
    case '(':
        advance(lexer);
        return token_init(TOK_LPAREN, span_from("("), curr_line, curr_col);
    case ')':
        advance(lexer);
        return token_init(TOK_RPAREN, span_from(")"), curr_line, curr_col);
    case '{':
        advance(lexer);
        return token_init(TOK_LBRACE, span_from("{"), curr_line, curr_col);
    case '}':
        advance(lexer);
        return token_init(TOK_RBRACE, span_from("}"), curr_line, curr_col);
    case ',':
        advance(lexer);
        return token_init(TOK_COMMA, span_from(","), curr_line, curr_col);
    case '-':
        advance(lexer);

        if (current(lexer) == '>') {
            advance(lexer); // skip '>'
            return token_init(TOK_ARROW, span_from("->"), curr_line, curr_col);
        }

        return token_init(TOK_MINUS, span_from("-"), curr_line, curr_col);
    default:
        break;
    }

    if (isalpha(current(lexer)) || current(lexer) == '_') {
        size_t length = 0;

        do {
            length++;
            advance(lexer);
        } while (!is_eof(lexer) && (isalnum(current(lexer)) || current(lexer) == '_'));

        Span span = span_init(curr_input, length);

        if (span_equals(span, span_from("i64")))
            return token_init(TOK_TYPEI64, span, curr_line, curr_col);

        if (span_equals(span, span_from("f64")))
            return token_init(TOK_TYPEF64, span, curr_line, curr_col);

        if (span_equals(span, span_from("bool")))
            return token_init(TOK_TYPEBOOL, span, curr_line, curr_col);

        if (span_equals(span, span_from("string")))
            return token_init(TOK_TYPESTRING, span, curr_line, curr_col);

        if (span_equals(span, span_from("let")))
            return token_init(TOK_LET, span, curr_line, curr_col);

        if (span_equals(span, span_from("return")))
            return token_init(TOK_RETURN, span, curr_line, curr_col);

        return token_init(TOK_IDENTIFIER, span, curr_line, curr_col);
    }

    if (isdigit(current(lexer))) {
        size_t length = 0;
        int is_double = 0;

        do {
            length++;
            advance(lexer);
        } while (!is_eof(lexer) && isdigit(current(lexer)));

        if (current(lexer) == '.') {
            length++;
            is_double = 1;
            advance(lexer);
        }

        size_t mantissa = 0;

        while (!is_eof(lexer) && isdigit(current(lexer))) {
            mantissa++;
            advance(lexer);
        }

        Span span = span_init(curr_input, length + mantissa);

        if (is_double && mantissa == 0) {
            fprintf(stderr, "(%zu:%zu) WARNING: invalid floating point number leads to garbage token\n", curr_line, curr_col);
            fprintf(stderr, "-> ");
            span_print(stderr, span);
            fprintf(stderr, "\n");

            return token_init(TOK_GARBAGE, span, curr_line, curr_col);
        }

        if (is_double)
            return token_init(TOK_DOUBLELITERAL, span, curr_line, curr_col);

        return token_init(TOK_INTLITERAL, span, curr_line, curr_col);
    }

    if (current(lexer) == '"') {
        advance(lexer); // skip '"'
        size_t length = 0;

        do {
            length++;
            advance(lexer);
        } while (!is_eof(lexer) && current(lexer) != '"');

        Span span = span_init(curr_input + 1, length);

        if (is_eof(lexer)) {
            fprintf(stderr, "(%zu:%zu) WARNING: invalid string literal leads to garbage token\n", curr_line, curr_col);
            fprintf(stderr, "-> ");
            span_print(stderr, span);
            fprintf(stderr, "\n");
        }

        advance(lexer); // skip '"'

        return token_init(TOK_STRINGLITERAL, span, curr_line, curr_col);
    }

    size_t length = 0;

    while (!is_eof(lexer) && !isspace(current(lexer))) {
        length++;
        advance(lexer);
    }

    return token_init(TOK_GARBAGE, span_init(curr_input, length), curr_line, curr_col);
}

static char current(Lexer* lexer) {
    return *lexer->input;
}

static char peek(Lexer* lexer) {
    return *(lexer->input + 1);
}

static int is_eof(Lexer* lexer) {
    return current(lexer) == 0;
}

static void advance(Lexer* lexer) {
    if (!is_eof(lexer)) {
        if (current(lexer) == '\n') {
            lexer->line++;
            lexer->col = 1;
        } else {
            lexer->col++;
        }

        lexer->input++;
    }
}

static void skip_whitespaces(Lexer* lexer) {
    while (!is_eof(lexer) && isspace(current(lexer)))
        advance(lexer);
}
