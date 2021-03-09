#include "lex.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

char* readLexemString() {
    size_t buffer_size = DEFAULT_BUFFER_SIZE;
    size_t end = 0;
    char* buffer = calloc(buffer_size, sizeof(*buffer));
    if (!buffer) {
        return NULL;
    }

    int c = 0;
    while ((c = getchar()) != EOF) {
        const char c_c = c;
        if (!append(&buffer, &buffer_size, sizeof(*buffer), &c_c, end)) {
            free(buffer);
            return NULL;
        }

        end++;
    }

    if (end == buffer_size) {
        const char e_c = '\0';
        if (!append(&buffer, &buffer_size, sizeof(*buffer), &e_c, end)) {
            free(buffer);
            return NULL;
        }
    }

    return buffer;
}

bool isBracket(const char c) {
    return c == '(' || c == ')';
}

bool isOperator(const char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

typedef struct {
    char const* str;
} LexState;

void lexStateAdvance(LexState* const state) {
    assert(state);
    assert(state->str);
    assert(state->str[0]);

    state->str++;
}

void lexSpace(LexState* const state) {
    assert(state);

    lexStateAdvance(state);
}

Lexem lexBracket(LexState* const state) {
    assert(state);

    const char c = state->str[0];
    assert(isBracket(c));
    const Lexem lex = {
        .type = LEXEM_TYPE_BRACKET,
        .bracket_type = (c == '(') ? (BRACKET_LEFT) : (BRACKET_RIGHT),
    };

    lexStateAdvance(state);

    return lex;
}

Lexem lexNumber(LexState* const state) {
    assert(state);
    assert(state->str);

    Lexem lex = {
        .type = LEXEM_TYPE_VALUE,
    };
    size_t len = 0;
    if (sscanf(state->str, "%d%zn", &lex.value, &len) != 1) {
        state->str = NULL;
    }

    state->str += len;

    return lex;
}

Lexem lexOperator(LexState* const state) {
    assert(state);

    const char c = state->str[0];
    assert(isOperator(c));
    Lexem lex = {
        .type = LEXEM_TYPE_OPERATION,
    };
    switch (c) {
        case '+':
            lex.operation_type = OPERATION_ADD;
            break;
        case '-':
            lex.operation_type = OPERATION_SUB;
            break;
        case '*':
            lex.operation_type = OPERATION_MUL;
            break;
        case '/':
            lex.operation_type = OPERATION_DIV;
            break;
        default:
            assert(!"Invalid operator");
    }

    lexStateAdvance(state);

    return lex;
}

Lexem* lex(char const* const str, size_t* const sz_p) {
    assert(str);
    assert(sz_p);

    size_t buffer_size = DEFAULT_BUFFER_SIZE;
    size_t end = 0;
    Lexem* buffer = calloc(buffer_size, sizeof(*buffer));
    if (!buffer) {
        return NULL;
    }

    LexState state = {
        str,
    };

    char c = 0;
    while ((c = state.str[0])) {
        if (isspace(c)) {
            lexSpace(&state);
        } else {
            Lexem lex;
            if (isBracket(c)) {
                lex = lexBracket(&state);
            } else if (isdigit(c)) {
                lex = lexNumber(&state);
            } else if (isOperator(c)) {
                lex = lexOperator(&state);
            } else {
                state.str = NULL;
            }

            if (!state.str || !append(&buffer, &buffer_size, sizeof(*buffer), &lex, end)) {
                state.str = NULL;
            }

            end++;
        }

        if (!state.str) {
            free(buffer);
            return NULL;
        }
    }

    *sz_p = end;

    return buffer;
}

void printLexem(Lexem l) {
    switch (l.type) {
        case LEXEM_TYPE_BRACKET:
            switch (l.bracket_type) {
                case BRACKET_LEFT:
                    printf("LBRAC");
                    break;
                case BRACKET_RIGHT:
                    printf("RBRAC");
                    break;
                default:
                    assert(!"Invalid enumeration value");
            }
            break;
        case LEXEM_TYPE_OPERATION:
            switch (l.operation_type) {
                case OPERATION_ADD:
                    printf("PLUS");
                    break;
                case OPERATION_SUB:
                    printf("MINUS");
                    break;
                case OPERATION_MUL:
                    printf("MUL");
                    break;
                case OPERATION_DIV:
                    printf("DIV");
                    break;
                default:
                    assert(!"Invalid enumeration value");
            }
            break;
        case LEXEM_TYPE_VALUE:
            printf("NUMBER:%d", l.value);
            break;
        default:
            assert(!"Invalid enumeration value");
    }
}

void printLexemArray(Lexem const* const arr, const size_t sz) {
    if (!arr || !sz) {
        printf("ERROR\n");
        return;
    }

    printLexem(arr[0]);
    for (size_t i = 1; i < sz; i++) {
        printf(" ");
        printLexem(arr[i]);
    }
    printf("\n");
}
