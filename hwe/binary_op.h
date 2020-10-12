#pragma once

#include <stdbool.h>

typedef unsigned long long ull;

void set_bit(unsigned char *p, ull n);
bool read_bit(unsigned char const *p, ull n);
void get_byte_bit(ull i, ull *byte, ull *bit);
