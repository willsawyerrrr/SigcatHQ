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

    struct sigaction toStdout;
    memset(&toStdout, 0, sizeof(toStdout));
    toStdout.sa_handler = setStreamStdout;
    toStdout.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &toStdout, 0);

    struct sigaction toStderr;
    memset(&toStderr, 0, sizeof(toStderr));
    toStderr.sa_handler = setStreamStderr;
    toStderr.sa_flags = SA_RESTART;
    sigaction(SIGUSR2, &toStderr, 0);

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
