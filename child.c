#include "child.h"

#include <csse2310a3.h>
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// longest possible status is null-terminated "signalled(XXX)" (15 characters)
#define MAX_STATUS_BUFFER_SIZE 15

/* Stores information about child processes created by the spawn command. */
extern ChildList* childList;

Child* get_child_by_jobid(int jobId) {
    Child** children = childList->children;
    for (int i = 0; children[i]; i++) {
        if (children[i]->jobId == jobId) {
            return children[i];
        }
    }

    return NULL;
}

Child* get_child_by_pid(int processId) {
    Child** children = childList->children;
    for (int i = 0; children[i]; i++) {
        if (children[i]->processId == processId) {
            return children[i];
        }
    }

    return NULL;
}

ChildList* init_child_list() {
    ChildList* childList = malloc(sizeof(ChildList));
    childList->numChildren = 0;
    Child** children = malloc(sizeof(Child*));
    childList->children = children;
    return childList;
}

Child* init_child(pid_t processId, char* programName, int pToC, int cToP) {
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
    child->readStream = fdopen(cToP, "r");
        
    // add the child to the list
    childList->children = realloc(childList->children,
            sizeof(Child) * (++(childList->numChildren)));
    childList->children[childList->numChildren - 1] = child;

    return child;
}

void report_single_child(Child* child) {
    wait_on_child(child);
    printf("[%d] %s:%s\n", child->jobId, child->programName, child->status);
    fflush(stdout);
}

void wait_on_child(Child* child) {
    int statusCode;
    char* status = child->status;
    
    // only wait on child and write to its status string if the last status
    // was running - if it has been changed already, it cannot change again
    if (!strcmp(status, "running")
            && waitpid(child->processId, &statusCode, WNOHANG)) {
        if (WIFEXITED(statusCode)) { // statusCode => exited
            sprintf(status, "exited(%d)", WEXITSTATUS(statusCode));
        } else if (WIFSIGNALED(statusCode)) { // statusCode => signalled
            sprintf(status, "signalled(%d)", WTERMSIG(statusCode));
        } else {
            strcpy(status, "running");
        }
    }
}

void reap_child(int signum, siginfo_t* info, void* ucontext) {
    Child* child = get_child_by_pid(info->si_pid);
    wait_on_child(child);
}

void free_child_list() {
    Child** children = childList->children;
    for (int i = 0; children[i]; i++) {
        Child* child = children[i];
        free(child->programName);
        free(child->status);
        fclose(child->readStream);
        free(child);
    }
    free(children);
    free(childList);
}

