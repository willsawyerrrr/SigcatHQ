#include "hq.h"

#include <csse2310a3.h>
#include <ctype.h>
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

#define EXIT_EXEC_FAIL 99
 
// longest possible status is null-terminated "signalled(XX)" (14 characters)
#define MAX_STATUS_BUFFER_SIZE 14

#define PIPE_WRITE_END 1
#define PIPE_READ_END 0

int main() {
    ignore_signals();

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
     
    return EXIT_SUCCESS;
}

void parse(char* command, ChildList* childList) {
    int numArgs;
    char** args = split_space_not_quote(command, &numArgs);

    if (!numArgs) {
        return;
    }
    
    char* program = args[0];

    if (!strcmp(program, "spawn")) {
        spawn(numArgs, args, childList);
    } else if (!strcmp(program, "report")) {
        report(numArgs, args, childList);
    } else if (!strcmp(program, "signal")) {
        send_signal(numArgs, args, childList);
    } else if (!strcmp(program, "sleep")) {
        sleep_hq(numArgs, args, childList);
    } else if (!strcmp(program, "send")) {
        send(numArgs, args, childList);
    } else if (!strcmp(program, "rcv")) {
        rcv(numArgs, args, childList);
    } else if (!strcmp(program, "eof")) {
        eof(numArgs, args, childList);
    } else if (!strcmp(program, "cleanup")) {
        cleanup(childList);
    } else {
        printf("Error: Invalid command\n");
    }

    free(args);
}

void ignore_signals() {
    struct sigaction ignore;
    memset(&ignore, 0, sizeof(ignore));
    ignore.sa_handler = SIG_IGN;
    ignore.sa_flags = SA_RESTART;
    sigaction(SIGINT, &ignore, 0);
    sigaction(SIGPIPE, &ignore, 0);
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
        close(pToC[PIPE_READ_END]); // close parent to child read end
        close(cToP[PIPE_WRITE_END]); // close child to parent write end

        Child* child = init_child(childList, childId, args[1],
                pToC[PIPE_WRITE_END], cToP[PIPE_READ_END]);
        
        printf("New Job ID [%d] created\n", child->jobId);
        fflush(stdout);
    } else { // child
        close(pToC[PIPE_WRITE_END]); // close parent to child write end
        close(cToP[PIPE_READ_END]); // close child to parent read end

        dup2(pToC[PIPE_READ_END], STDIN_FILENO);
        dup2(cToP[PIPE_WRITE_END], STDOUT_FILENO);
        
        execvp(args[1], &args[1]);

        // this point is only reached if the above exec() call failed
        exit(EXIT_EXEC_FAIL);
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
    int statusCode;
    char* status = child->status;
    
    // Only write to child's status string if the last status was running;
    // otherwise, the last assigned status was exited or signalled, both of
    // which cannot change. Hence, the previous status is reused.
    if (!strcmp(status, "running")
            && waitpid(child->processId, &statusCode, WNOHANG)) {
        if (WIFEXITED(statusCode)) { // statusCode => exited
            sprintf(status, "exited(%d)", WEXITSTATUS(statusCode));
        } else if (WIFSIGNALED(statusCode)) { // statusCode => was sent signal
            sprintf(status, "signalled(%d)", WTERMSIG(statusCode));
        } else {
            strcpy(status, "running");
        }
    }

    printf("[%d] %s:%s\n", child->jobId, child->programName, status);
    fflush(stdout);
}

int validate_report_args(int numArgs, char** args, ChildList* childList) {
    return (numArgs == 1 || validate_jobid(args[1], childList));
}

void send_signal(int numArgs, char** args, ChildList* childList) {
    if (!validate_signal_args(numArgs, args, childList)) {
        return;
    }

    Child* child = get_child_by_jobid(atoi(args[1]), childList);
    int signum = atoi(args[2]);

    kill(child->processId, signum);
}

int validate_signal_args(int numArgs, char** args, ChildList* childList) {
    return (
            validate_num_args(SIGNAL_MIN_EXP_ARGS, numArgs)
            && validate_jobid(args[1], childList)
            && validate_signum(args[2])
            );
}

void sleep_hq(int numArgs, char** args, ChildList* childList) {
    if (!validate_sleep_args(numArgs, args, childList)) {
        return;
    }

    sleep(strtod(args[1], NULL));
}

int validate_sleep_args(int numArgs, char** args, ChildList* childList) {
    if (!validate_num_args(SLEEP_MIN_EXP_ARGS, numArgs)) {
        return 0;
    } else if (!validate_numerical_arg(args[1], 1)
            || (strtod(args[1], NULL) < 0)) {
        printf("Error: Invalid sleep time\n");
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
            && validate_jobid(args[1], childList)
            );
}

void rcv(int numArgs, char** args, ChildList* childList) {
    if (!validate_rcv_args(numArgs, args, childList)) {
        return;
    }

    int jobId = atoi(args[1]);
    Child* child = get_child_by_jobid(jobId, childList);
    int cToP = child->cToP;

    if (!is_ready(cToP)) {
        printf("<no input>\n");
        fflush(stdout);
        return;
    }

    char* input = read_line(fdopen(cToP, "r"));
    if (input) {
        printf("%s\n", input);
        free(input);
    } else {
        printf("<EOF>\n");
    }

    fflush(stdout);
}

int validate_rcv_args(int numArgs, char** args, ChildList* childList) {
    return (validate_num_args(RCV_MIN_EXP_ARGS, numArgs)
            && validate_jobid(args[1], childList)
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
            && validate_jobid(args[1], childList)
            );
}

void cleanup(ChildList* childList) {
    Child** children = childList->children;
    for (int i = 0; children[i]; i++) {
        pid_t processId = children[i]->processId;
        kill(processId, SIGKILL);
        waitpid(processId, NULL, 0);
    }
}

int validate_num_args(int minExpected, int given) {
    if (given >= minExpected) {
        return 1;
    }
    printf("Error: Insufficient arguments\n");
    fflush(stdout);
    return 0;
}

int validate_numerical_arg(char* arg, int allowFractional) {
    // skip leading whitespace by moving the pointer to the beginning of arg
    while (arg[0] == ' ') {
        arg++;
    }

    for (int i = 0; arg[i]; i++) {
        if (!isdigit(arg[i])
                && (!allowFractional || arg[i] != '.')) {
            return 0;
        }
    }
    
    // if arg is empty, the loop will be skipped and we'll make it here, so
    // only return 1 (true) if the string is non-zero
    return strlen(arg); 
}

int validate_jobid(char* jobId, ChildList* childList){
    if (validate_numerical_arg(jobId, 0)
            && (atoi(jobId) < childList->numChildren)) {
        return 1;
    }
    printf("Error: Invalid job\n");
    fflush(stdout);
    return 0;
}

int validate_signum(char* signum) {
    if (validate_numerical_arg(signum, 0)
            && atoi(signum) >= 1 && atoi(signum) <= 31) {
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

Child* init_child(ChildList* childList, pid_t processId, char* programName,
        int pToC, int cToP) {
    // create child
    Child* child = malloc(sizeof(Child));
    child->processId = processId;
    child->jobId = childList->numChildren;
    child->programName = malloc(strlen(programName) + 1);
    strcpy(child->programName, programName);
    child->status = malloc(MAX_STATUS_BUFFER_SIZE);
    strcpy(child->status, "running");
    child->pToC = pToC;
    child->cToP = cToP;
        
    // add the child to the list
    childList->children = realloc(childList->children,
            sizeof(Child) * (++(childList->numChildren)));
    childList->children[childList->numChildren - 1] = child;

    return child;
}

void free_child_list(ChildList* childList) {
    Child** children = childList->children;
    for (int i = 0; children[i]; i++) {
        Child* child = children[i];
        free(child->programName);
        free(child->status);
        free(child);
    }
    free(children);
    free(childList);
}

