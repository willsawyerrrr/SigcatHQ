#include "hq.h"

#include <csse2310a3.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    char* input;

    do {
        printf("> ");
        input = read_line(stdin);
        parse(input);
        free(input);
    } while (!feof(stdin));
        
    return 0;
}

void parse(char* command) {
    printf("%s\n", command);
}

void spawn() {
}

void report() {
}

void signal() {
}

void sleep() {
}

void send() {
}

void rcv() {
}

void eof() {
}

void cleanup() {
}

