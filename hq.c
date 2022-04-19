#include "hq.h"

#include <csse2310a3.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define EOF_MIN_EXP_ARGS 2
#define RCV_MIN_EXP_ARGS 2
#define SEND_MIN_EXP_ARGS 3
#define SIGNAL_MIN_EXP_ARGS 3
#define SLEEP_MIN_EXP_ARGS 2
#define SPAWN_MIN_EXP_ARGS 2

int main() {
    ignore_interrupt();

    ChildList* childList = malloc(sizeof(ChildList*));
    childList->numChildren = 0;
    Child** children = malloc(sizeof(Child*));
    childList->children = children;

    char* input;
    printf("> ");
    fflush(stdout);
    input = read_line(stdin);
    while (input) { // while input != NULL
        if (strcmp(input, "")) { // if input is non-empty
            parse(input, childList);
        }
        free(input);
        printf("> ");
        fflush(stdout);
        input = read_line(stdin);
    }

    for (int i = 0; childList->children[i]; i++) {
        free(childList->children[i]);
    }
    free(childList);
     
    return 0;
}

void parse(char* command, ChildList* childList) {
    int* numArgs = malloc(sizeof(int));
    char** args = split_space_not_quote(command, numArgs);

    if (!*numArgs) {
        return;
    }
    
    char* program = args[0];

    if (!strcmp(program, "spawn")) {
        spawn(*numArgs, args, childList);
    } else if (!strcmp(program, "report")) {
        report(*numArgs, args, childList);
    } else if (!strcmp(program, "signal")) {
        send_signal(*numArgs, args, childList);
    } else if (!strcmp(program, "sleep")) {
        sleep_hq(*numArgs, args, childList);
    } else if (!strcmp(program, "send")) {
        send(*numArgs, args, childList);
    } else if (!strcmp(program, "rcv")) {
        rcv(*numArgs, args, childList);
    } else if (!strcmp(program, "eof")) {
        eof(*numArgs, args, childList);
    } else if (!strcmp(program, "cleanup")) {
        cleanup();
    } else {
        printf("Error: Invalid command\n");
    }

    free(numArgs);
    free(args);
}

void ignore_interrupt() {
    struct sigaction ignore;
    memset(&ignore, 0, sizeof(ignore));
    ignore.sa_handler = SIG_IGN;
    ignore.sa_flags = SA_RESTART;
    sigaction(SIGINT, &ignore, 0);
}

void spawn(int numArgs, char** args, ChildList* childList) {
    if (!validate_spawn_args(numArgs, args, childList)) {
        return;
    }
}

int validate_spawn_args(int numArgs, char** args, ChildList* childList) {
    if (!validate_num_args(SPAWN_MIN_EXP_ARGS, numArgs)) {
        return 0;
    }

    // check args
    return 0;
}

void report(int numArgs, char** args, ChildList* childList) {
}

int validate_report_args(int numArgs, char** args, ChildList* childList) {
    // check args
    return 0;
}

void send_signal(int numArgs, char** args, ChildList* childList) {
    if (!validate_signal_args(numArgs, args, childList)) {
        return;
    }
}

int validate_signal_args(int numArgs, char** args, ChildList* childList) {
    if (!validate_num_args(SIGNAL_MIN_EXP_ARGS, numArgs)) {
        return 0;
    }

    // check args
    return 0;
}

void sleep_hq(int numArgs, char** args, ChildList* childList) {
    if (!validate_sleep_args(numArgs, args, childList)) {
        return;
    }
}

int validate_sleep_args(int numArgs, char** args, ChildList* childList) {
    if (!validate_num_args(SLEEP_MIN_EXP_ARGS, numArgs)) {
        return 0;
    }

    // check args
    return 0;
}

void send(int numArgs, char** args, ChildList* childList) {
    if (!validate_send_args(numArgs, args, childList)) {
        return;
    }
}

int validate_send_args(int numArgs, char** args, ChildList* childList) {
    if (!validate_num_args(SEND_MIN_EXP_ARGS, numArgs)) {
        return 0;
    }

    // check args
    return 0;
}

void rcv(int numArgs, char** args, ChildList* childList) {
    if (!validate_rcv_args(numArgs, args, childList)) {
        return;
    }
}

int validate_rcv_args(int numArgs, char** args, ChildList* childList) {
    if (!validate_num_args(RCV_MIN_EXP_ARGS, numArgs)) {
        return 0;
    }

    // check args
    return 0;
}

void eof(int numArgs, char** args, ChildList* childList) {
    if (!validate_eof_args(numArgs, args, childList)) {
        return;
    }
}

int validate_eof_args(int numArgs, char** args, ChildList* childList) {
    if (!validate_num_args(EOF_MIN_EXP_ARGS, numArgs)) {
        return 0;
    }

    // check args
    return 0;
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

int validate_job_id(int jobId, ChildList* childList) {
    Child** children = childList->children;
    for (int i = 0; children[i]; i++) {
        if (children[i]->jobId == jobId) {
            return 1;
        }
    }
    printf("Error: Invalid job\n");
    return 0;
}

int validate_numerical_arg(char* arg) {
    return 0; // no args are numerical
}

