#include "sigcat.h"

#include <csse2310a3.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE* outputStream;

int main() {
    char* input;
    outputStream = stdout;

    set_handlers();

    do {
        input = read_line(stdin);
        if (input == NULL) {
            fprintf(outputStream, "\n");
            break;
        }
        fprintf(outputStream, "%s\n", input);
        free(input);
    } while (!feof(stdin));

    return 0;
}

void set_handlers() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sa.sa_flags = SA_RESTART;

    for (int signum = 1; signum <= 31; signum++) {
        sigaction(signum, &sa, 0);
    }
}

void handler(int signum) {
    fprintf(outputStream, "sigcat received %s\n", strsignal(signum));
    fflush(outputStream);

    if (signum == 10) {
        outputStream = stdout;
    } else if (signum == 12) {
        outputStream = stderr;
    }
}

