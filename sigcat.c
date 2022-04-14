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

    setHandlers();

    do {
        input = read_line(stdin);
        fprintf(outputStream, input);
        free(input);
    } while (!feof(stdin));

    return 0;
}

void setHandlers() {
    // set handler for all signals (1 to 31) to handler method
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

