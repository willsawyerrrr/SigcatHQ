#define _GNU_SOURCE

#include "hq.h"

#include <csse2310a3.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define EOF_MIN_EXP_ARGS 2
#define RCV_MIN_EXP_ARGS 2
#define SEND_MIN_EXP_ARGS 3
#define SIGNAL_MIN_EXP_ARGS 3
#define SLEEP_MIN_EXP_ARGS 2
#define SPAWN_MIN_EXP_ARGS 2

int main() {
    ignore_interrupt();

    ChildList* childList = init_child_list();

    char* input;
    printf("> ");
    fflush(stdout);
    input = read_line(stdin);
    while (input) { // haven't received EOF
        if (strcmp(input, "")) { // if input is non-empty
            parse(input, childList);
        }
        free(input);
        printf("> ");
        fflush(stdout);
        input = read_line(stdin);
    }

    cleanup(childList);
    free_child_list(childList);
     
    return 0;
}

void parse(char* command, ChildList* childList) {
    int* numArgs = malloc(sizeof(int));
    char** args = split_space_not_quote(command, numArgs);

    if (!*numArgs) {
        free(numArgs);
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
        cleanup(childList);
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

    int pToC[2];
    pipe(pToC); // parent to child

    int cToP[2];
    pipe(cToP); // child to parent

    pid_t childId = fork();
    if (childId) { // parent
        close(pToC[0]); // close parent to child read end
        close(cToP[1]); // close child to parent write end

        // create new child struct
        Child* child = malloc(sizeof(Child));
        child->processId = childId;
        child->jobId = childList->numChildren;
        child->programName= malloc(sizeof(char));
        strcpy(child->programName, args[1]);
        child->pToC = pToC[1];
        child->cToP = cToP[0];
        
        // add the child to the list
        childList->children = realloc(childList->children,
                sizeof(Child) * (childList->numChildren + 1));
        childList->children[childList->numChildren] = child;
        childList->numChildren++;

        printf("New Job ID [%d] created\n", child->jobId);
        fflush(stdout);
    } else { // child
        close(pToC[1]); // close parent to child write end
        close(cToP[0]); // close child to parent read end

        dup2(pToC[0], STDIN_FILENO);
        dup2(cToP[1], STDOUT_FILENO);
        
        execvp(args[1], args);

        // this point is only reached if the above exec() call failed
        exit(99);
    }
}

int validate_spawn_args(int numArgs, char** args, ChildList* childList) {
    return validate_num_args(SPAWN_MIN_EXP_ARGS, numArgs);
}

void report(int numArgs, char** args, ChildList* childList) {
    if (!validate_report_args(numArgs, args, childList)) {
        return;
    }
    Child** children = childList->children;
    printf("[Job] cmd:status\n");
    if (numArgs > 1) {
        Child* child = get_child_by_jobid(atoi(args[1]), childList);
        report_single(child);
    } else {
        for (int i = 0; children[i]; i++) {
            report_single(children[i]);
        }
    }
}

void report_single(Child* child) {
    int statusCode = 0;
    char* status = malloc(sizeof(char));
    
    if (waitpid(child->processId, &statusCode, WNOHANG) != -1) {
        if (WIFEXITED(statusCode)) { // statusCode => exited
            sprintf(status, "exited(%d)", WEXITSTATUS(statusCode));
        } else if (WIFSIGNALED(statusCode)) { // statusCode => was sent signal
            sprintf(status, "signalled(%d)", WTERMSIG(statusCode));
        } else {
            strcpy(status, "running");
        }
        printf("[%d] %s:%s\n", child->jobId, child->programName, status);
        fflush(stdout);
    }

    free(status);
}

int validate_report_args(int numArgs, char** args, ChildList* childList) {
    return (
            numArgs == 1
            || (validate_numerical_arg(args[1])
                && validate_jobid(atoi(args[1]), childList))
           );
}

void send_signal(int numArgs, char** args, ChildList* childList) {
    if (!validate_signal_args(numArgs, args, childList)) {
        return;
    }
}

int validate_signal_args(int numArgs, char** args, ChildList* childList) {
    return (
            validate_num_args(SIGNAL_MIN_EXP_ARGS, numArgs)
            && validate_numerical_arg(args[1])
            && validate_jobid(atoi(args[1]), childList)
            && validate_numerical_arg(args[2])
            && validate_signum(atoi(args[2]))
           );
}

void sleep_hq(int numArgs, char** args, ChildList* childList) {
    if (!validate_sleep_args(numArgs, args, childList)) {
        return;
    }
}

int validate_sleep_args(int numArgs, char** args, ChildList* childList) {
    if (!validate_num_args(SLEEP_MIN_EXP_ARGS, numArgs)) {
        return 0;
    } else if (!validate_numerical_arg(args[1])
            || (strtod(args[1], NULL) < 0)) {
        printf("Error: Invalid sleep time\b");
        fflush(stdout);
        return 0;
    }

    return 1;
}

void send(int numArgs, char** args, ChildList* childList) {
    if (!validate_send_args(numArgs, args, childList)) {
        return;
    }
    
    int jobId = atoi(args[1]);
    Child* child = get_child_by_jobid(jobId, childList);
    write(child->pToC, args[2], strlen(args[2]) + 1);
    write(child->pToC, "\n", 2);
}

int validate_send_args(int numArgs, char** args, ChildList* childList) {
    return (
            validate_num_args(SEND_MIN_EXP_ARGS, numArgs)
            && validate_numerical_arg(args[1])
            && validate_jobid(atoi(args[1]), childList)
           );
}

void rcv(int numArgs, char** args, ChildList* childList) {
    if (!validate_rcv_args(numArgs, args, childList)) {
        return;
    }

    int jobId = atoi(args[1]);
    Child* child = get_child_by_jobid(jobId, childList);
    int cToP = child->cToP;

    char* readBuffer = malloc(sizeof(char));
    if (!is_ready(cToP)) {
        printf("<no input>");
    } else if (!read(child->cToP, readBuffer, 1)) {
        printf("<EOF>");
    } else {
        do {
            printf("%s", readBuffer);
        } while (read(child->cToP, readBuffer, 1));
    }

    free(readBuffer);
    printf("\n");
    fflush(stdout);
}

int validate_rcv_args(int numArgs, char** args, ChildList* childList) {
    return (validate_num_args(RCV_MIN_EXP_ARGS, numArgs)
            && validate_numerical_arg(args[1])
            && validate_jobid(atoi(args[1]), childList)
           );
}

void eof(int numArgs, char** args, ChildList* childList) {
    if (!validate_eof_args(numArgs, args, childList)) {
        return;
    }

    int jobId = atoi(args[1]);
    Child* child = get_child_by_jobid(jobId, childList);
    close(child->pToC);
}

int validate_eof_args(int numArgs, char** args, ChildList* childList) {
    return (validate_num_args(EOF_MIN_EXP_ARGS, numArgs)
            && validate_numerical_arg(args[1])
            && validate_jobid(atoi(args[1]), childList)
           );
}

void cleanup(ChildList* childList) {
    Child** children = childList->children;
    for (int i = 0; children[i]; i++) {
        // send signal -9 to each child
    }
}

int validate_num_args(int minExpected, int given) {
    if (given >= minExpected) {
        return 1;
    }
    printf("Error: insufficient arguments\n");
    fflush(stdout);
    return 0;
}

int validate_numerical_arg(char* arg) {
    char** next = malloc(sizeof(char*));
    strtod(arg, next);
    return **next == '\0';
}

int validate_jobid(int jobId, ChildList* childList){
    if (get_child_by_jobid(jobId, childList)) {
        return 1;
    }
    printf("Error: Invalid job\n");
    fflush(stdout);
    return 0;
}

int validate_signum(int signum) {
    if (signum >= 1 && signum <= 31) {
        return 1;
    }
    printf("Error: Invalid signal\n");
    fflush(stdout);
    return 0;
}

Child* get_child_by_jobid(int jobId, ChildList* childList) {
    Child** children = childList->children;
    for (int i = 0; children[i]; i++) {
        if (children[i]->jobId == jobId) {
            return children[i];
        }
    }

    return NULL;
}

ChildList* init_child_list() {
    ChildList* childList = malloc(sizeof(ChildList));
    childList->numChildren = 0;
    Child** children = malloc(sizeof(Child));
    childList->children = children;
    return childList;
}


void free_child_list(ChildList* childList) {
    Child** children = childList-> children;
    for (int i = 0; children[i]; i++) {
        free(children[i]->programName);
        free(children[i]);
    }
    free(childList);
}

