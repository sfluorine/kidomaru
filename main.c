#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

#define ERR_FILE_EMPTY (char*)0xDEADBEEF
#define ERR_FILE_MISREAD (char*)0xBEEFDEAD /* NOTE: this error name is kinda misleading. this error will yield when num of read bytes is not equal to ftell's size. */

static void usage(const char* program);
static char* read_whole_file(const char* filepath);

int main(int argc, char** argv) {
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    const char* filepath = argv[1];
    char* file_contents = read_whole_file(filepath);

    if (file_contents == NULL) {
        fprintf(stderr, "ERROR: cannot open '%s'!\n", filepath);
        return 1;
    } 

    if (file_contents == ERR_FILE_EMPTY)
        return 0;

    Lexer lexer = lexer_init(file_contents);
    Parser parser = parser_init(&lexer);

    
    Statement* root = parse_statement(&parser);
    Interpreter interpreter = interpreter_init(root);

    interpreter_begin(&interpreter);
    interpreter_deinit(&interpreter);

    free(file_contents);

    return 0;
}

static void usage(const char* program) {
    fprintf(stderr, "Usage: %s <file>\n", program);
    fprintf(stderr, "No input files was provided!\n");
}

static char* read_whole_file(const char* filepath) {
    if (!filepath)
        return NULL;

    FILE* file = fopen(filepath, "r");
    
    if (!file)
        return NULL;

    fseeko(file, 0, SEEK_END);
    off_t size = ftell(file);
    fseeko(file, 0, SEEK_SET);

    if (size == 0) {
        fclose(file);
        return ERR_FILE_EMPTY;
    }

    char* buffer = malloc(sizeof(char) * size);
    size_t nitems = fread(buffer, sizeof(char), size, file);

    fclose(file);

    if (((size_t)size) != nitems) {
        free(buffer);
        return ERR_FILE_MISREAD;
    }

    // add null terminating character.
    buffer[size - 1] = 0;

    return buffer;
}
