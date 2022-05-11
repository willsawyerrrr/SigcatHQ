#ifndef CHILD_H
#define CHILD_H

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>

/* Stores information used to manipulate processes. */
typedef struct {
    /* Process ID of this process, relative to the kernel. */
    pid_t processId;
    /* Job ID of this process, relative to hq. */
    pid_t jobId;
    /* Name of the program running within this process. */
    char* programName;
    /* Status of this process: running, exited or signalled. */
    char* status;
    /* Pipe used to write to this process. */
    int pToC;
    /* Pipe used to query information from this child. */
    int cToP;
    /* Stream used to read from this child. */
    FILE* readStream;
} Child;

/* Stores multiple Child processes. */
typedef struct {
    /* Number of Child processes being stored. */
    int numChildren;
    /* Array of child processes being stored. */
    Child** children;
} ChildList;

/*
 * Returns a pointer to the child process specified by the given job ID; a NULL
 * pointer is returned if no child has the given job ID.
 */
Child* get_child_by_jobid(int jobId);

/*
 * Returns a pointer to the child process specified by the given process ID; a
 * NULL pointer is returned if no child has the given process ID.
 */
Child* get_child_by_pid(int processId);

/*
 * Returns a pointer to an empty ChildList object with an empty array of
 * children and number of children initialised to 0.
 *
 * The returned ChildList and its array of children are allocated using
 * malloc(). It is the caller's responsibility to free these allocations.
 */
ChildList* init_child_list();

/*
 * Returns a pointer to a new Child object with the given IDs, program name and
 * communication pipes.
 *
 * The returned Child, its program name, and its status are each allocated by
 * malloc(). It is the caller's responsibility to free these allocations.
 */
Child* init_child(pid_t processId, char* programName, int pToC, int cToP);

/*
 * Prints a report on the given child process's status. The format of the
 * printed string is:
 *      [Job] cmd:status
 * Where Job is the jobId of the process, cmd is the name of the program the
 * process is executing and status is the status of the process and is either:
 *      "running"           ;
 *      "exited(code)"      , where code is the process's exit code; or
 *      "signalled(signum)" , where signum is the number of the signal which
 *                            terminated the process.
 */
void report_single_child(Child* child);

/*
 * Waits on the given child.
 *
 * If the child's status has not changed since it was last waited on (or since
 * it was created, if it has not yet been waited on), the child's status is
 * unchanged.
 *
 * If the child has exited since last it was waited on, the child's status is
 * changed to "exited(X)", where X is the child's exit code and possibly a
 * two-digit number.
 *
 * If the child has been signalled, resulting in termination, since it was last
 * waited on, the child's status is changed to "signalled(X)", where X is the
 * number of the signal sent to the child.
 */
void wait_on_child(Child* child);

/*
 * Signal handler to reap a child which sends a SIGCHLD signal.
 */
void reap_child(int signum, siginfo_t* info, void* ucontext);

/*
 * Frees the global ChildList and all of its children.
 */
void free_child_list();

/*
 * Frees the given Child and all of its members.
 */
void free_child(Child* child);

#endif
