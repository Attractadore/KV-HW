#pragma once

#include <stddef.h>

typedef enum {
    BRACKET_LEFT,
    BRACKET_RIGHT,
} BracketType;

typedef enum {
    OPERATION_ADD,
    OPERATION_SUB,
    OPERATION_MUL,
    OPERATION_DIV,
} OperationType;

typedef int ValueType;

typedef enum {
    LEXEM_TYPE_VALUE,
    LEXEM_TYPE_OPERATION,
    LEXEM_TYPE_BRACKET,
} LexemType;

typedef struct {
    LexemType type;
    union {
        BracketType bracket_type;
        OperationType operation_type;
        ValueType value;
    };
} Lexem;

typedef enum {
    NODE_TYPE_VALUE,
    NODE_TYPE_OPERATOR,
} NodeType;

typedef struct Node_T {
    NodeType node_type;
    union {
        ValueType value;
        struct {
            OperationType operation_type;
            struct Node_T *left, *right;
        };
    };
} Node;

enum {
    DEFAULT_BUFFER_SIZE = 1024,
};

void* append(void** buffer_p, size_t* num_p, size_t sz, void const* elem, size_t end);
