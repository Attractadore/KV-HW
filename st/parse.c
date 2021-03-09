#include "defs.h"

#include <assert.h>
#include <stdlib.h>

void freeNode(Node* const top) {
    if (top) {
        if (top->node_type == NODE_TYPE_OPERATOR) {
            freeNode(top->left);
            freeNode(top->right);
        }
        free(top);
    }
}

ValueType eval(Node const* const top) {
    assert(top);

    switch (top->node_type) {
        case NODE_TYPE_VALUE:
            return top->value;
        case NODE_TYPE_OPERATOR: {
            assert(top->right && top->left);
            const ValueType right_value = eval(top->right);
            const ValueType left_value = eval(top->left);
            switch (top->operation_type) {
                case OPERATION_ADD:
                    return left_value + right_value;
                case OPERATION_SUB:
                    return left_value - right_value;
                case OPERATION_MUL:
                    return left_value * right_value;
                case OPERATION_DIV:
                    return left_value / right_value;

                default:
                    assert(!"Invalid enumeration value");
            }
        }
        default:
            assert(!"Invalid enumeration value");
    }
}

typedef struct {
    Lexem const* lexems;
    size_t num;
} ParseState;

Node* parseExpr(ParseState* state);
Node* parseMult(ParseState* state);
Node* parseTerm(ParseState* state);

void parseStateAdvance(ParseState* const state) {
    assert(state);
    assert(state->num);

    state->lexems++;
    state->num--;
}

typedef Node* (*ParseFunc)(ParseState* state);

Node* parseBase(ParseState* const state, const ParseFunc func, const OperationType op1, const OperationType op2) {
    assert(state);
    assert(func);

    Node* l = func(state);
    if (!l) {
        return NULL;
    }

    while (state->num && state->lexems->type == LEXEM_TYPE_OPERATION) {
        const OperationType operation_type = state->lexems->operation_type;
        if (operation_type != op1 && operation_type != op2) {
            break;
        }

        parseStateAdvance(state);

        Node* const r = func(state);
        if (!r) {
            freeNode(l);
            return NULL;
        }

        Node* const top = calloc(1, sizeof(*top));
        if (!top) {
            freeNode(l);
            freeNode(r);
            return NULL;
        }

        top->node_type = NODE_TYPE_OPERATOR;
        top->operation_type = operation_type;
        top->left = l;
        top->right = r;

        l = top;
    }

    return l;
}

Node* parseExpr(ParseState* const state) {
    assert(state);

    return parseBase(state, parseMult, OPERATION_ADD, OPERATION_SUB);
}

Node* parseMult(ParseState* const state) {
    assert(state);

    return parseBase(state, parseTerm, OPERATION_MUL, OPERATION_DIV);
}

Node* parseTerm(ParseState* const state) {
    assert(state);

    if (!state->num) {
        return NULL;
    }

    if (state->lexems->type == LEXEM_TYPE_VALUE) {
        Node* const top = calloc(1, sizeof(*top));
        if (!top) {
            return NULL;
        }

        top->node_type = NODE_TYPE_VALUE;
        top->value = state->lexems->value;

        parseStateAdvance(state);

        return top;
    }

    if (state->lexems->type != LEXEM_TYPE_BRACKET || state->lexems->bracket_type != BRACKET_LEFT) {
        return NULL;
    }
    parseStateAdvance(state);

    Node* const top = parseExpr(state);
    if (!top) {
        return NULL;
    }

    if (!state->num || state->lexems->type != LEXEM_TYPE_BRACKET || state->lexems->bracket_type != BRACKET_RIGHT) {
        freeNode(top);
        return NULL;
    }

    parseStateAdvance(state);

    return top;
}

Node* parse(Lexem const* const lexems, const size_t num) {
    assert(lexems);

    ParseState state = {
        lexems,
        num,
    };

    Node* const res = parseExpr(&state);

    if (state.num) {
        freeNode(res);
        return NULL;
    }

    return res;
}
