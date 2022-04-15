#include "hq.h"

#include <csse2310a3.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    char* input;

    do {
        printf("> ");
        input = read_line(stdin);
        free(input);
    } while (!feof(stdin));
        
    return 0;
}

