#include "sigcat.h"

#include <csse2310a3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE* outputStream;

int main() {
    char* input;
    outputStream = stdout;

    do {
        input = read_line(stdin);
        fprintf(outputStream, input);
        free(input);
    } while (!feof(stdin));

    return 0;
}

void setStreamStderr() {
    outputStream = stderr;
}

void setStreamStdout() {
    outputStream = stdout;
}
