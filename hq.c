#include "hq.h"

#include <csse2310a3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char* input;

    do {
        printf("> ");
        input = read_line(stdin);
        parse(input);
    } while (!feof(stdin));
     
    return 0;
}

void parse(char* command) {
    int* numArgs = malloc(sizeof(int));
    char** args = split_space_not_quote(command, numArgs);

    char* program = args[0];

    if (!strcmp(program, "spawn")) {
        spawn(*numArgs, args, command);
    } else if (!strcmp(program, "report")) {
        report(*numArgs, args, command);
    } else if (!strcmp(program, "signal")) {
        send_signal(*numArgs, args, command);
    } else if (!strcmp(program, "sleep")) {
        sleep(*numArgs, args, command);
    } else if (!strcmp(program, "send")) {
        send(*numArgs, args, command);
    } else if (!strcmp(program, "rcv")) {
        rcv(*numArgs, args, command);
    } else if (!strcmp(program, "eof")) {
        eof(*numArgs, args, command);
    } else if (!strcmp(program, "cleanup")) {
        cleanup(*numArgs, args, command);
    } else {
        printf("Error: Invalid command\n");
    }

    for (int i = 0; i < *numArgs - 1; i++) {
        free(args[i]);
    }
    free(numArgs);
    free(args);
}

void spawn(int numArgs, char** args, char* command) {
}

void report(int numArgs, char** args, char* command) {
}

void send_signal(int numArgs, char** args, char* command) {
}

void sleep(int numArgs, char** args, char* command) {
}

void send(int numArgs, char** args, char* command) {
}

void rcv(int numArgs, char** args, char* command) {
}

void eof(int numArgs, char** args, char* command) {
}

void cleanup(int numArgs, char** args, char* command) {
}

