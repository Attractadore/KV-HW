#pragma once

#include "defs.h"

char* readLexemString();

Lexem* lex(char const* const str, size_t* const sz_p);

void printLexem(Lexem l);
void printLexemArray(Lexem const* arr, size_t sz);
