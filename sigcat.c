#include <csse2310a3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char* input;
    FILE* outputStream = stdout;

    do {
        input = read_line(stdin);
        fprintf(outputStream, input);
        free(input);
    } while (!feof(stdin));

    return 0;
}
