#include "hq.h"

#include <csse2310a3.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char* input;

    ignoreInterrupt();

    do {
        printf("> ");
        input = read_line(stdin);
        if (input == NULL) {
            cleanup();
            fprintf(stdout, "\n");
            break;
        } else if (!strcmp(input, "")) {
            continue;
        }
        parse(input);
    } while (!feof(stdin));
     
    return 0;
}

void parse(char* command) {
    int* numArgs = malloc(sizeof(int));
    char** args = split_space_not_quote(command, numArgs);

    if (!*numArgs) {
        return;
    }
    
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
        cleanup();
    } else {
        printf("Error: Invalid command\n");
    }

    free(numArgs);
    free(args);
}

void ignoreInterrupt() {
    struct sigaction ignore;
    memset(&ignore, 0, sizeof(ignore));
    ignore.sa_handler = SIG_IGN;
    ignore.sa_flags = SA_RESTART;
    sigaction(SIGINT, &ignore, 0);
}

void spawn(int numArgs, char** args, char* command) {
    if (!validate_num_args(2, numArgs)) {
        return;
    }
}

void report(int numArgs, char** args, char* command) {
}

void send_signal(int numArgs, char** args, char* command) {
    if (!validate_num_args(3, numArgs)) {
        return;
    }
}

void sleep(int numArgs, char** args, char* command) {
    if (!validate_num_args(2, numArgs)) {
        return;
    }
}

void send(int numArgs, char** args, char* command) {
    if (!validate_num_args(3, numArgs)) {
        return;
    }
}

void rcv(int numArgs, char** args, char* command) {
    if (!validate_num_args(2, numArgs)) {
        return;
    }
}

void eof(int numArgs, char** args, char* command) {
    if (!validate_num_args(2, numArgs)) {
        return;
    }
}

void cleanup() {
}

int validate_num_args(int minExpected, int given) {
    if (given >= minExpected) {
        return 1;
    }
    printf("Error: insufficient arguments\n");
    return 0;
}

int validate_job_id(int jobId) {
    printf("Error: Invalid job\n");
    return 0; // all jobIds are invalid
}

