#include "lex.h"
#include "parse.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
    char* const lexems_str = readLexemString();
    if (!lexems_str) {
        return -1;
    }

    size_t lexems_sz = 0;
    Lexem* const lexems = lex(lexems_str, &lexems_sz);
    if (!lexems) {
        printf("ERROR\n");
    } else {
        Node* const parse_tree = parse(lexems, lexems_sz);
        if (!parse_tree) {
            printf("ERROR\n");
        } else {
            printf("%d\n", eval(parse_tree));
        }

        freeNode(parse_tree);
    }

    free(lexems);
    free(lexems_str);

    return 0;
}
