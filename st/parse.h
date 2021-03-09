#pragma once

#include "defs.h"

void freeNode(Node* top);

Node* parse(Lexem const* lexems, size_t num);
ValueType eval(Node const* top);
