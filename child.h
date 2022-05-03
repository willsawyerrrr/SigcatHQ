#ifndef CHILD_H
#define CHILD_H

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>

/*
 * Structure type to hold a child process - created within spawn.
 */
typedef struct {
    /* Process ID of the newly created child, relative to the kernel. */
    pid_t processId;
    /* Job ID of the newly created child, relative to hq. */
    pid_t jobId;
    /* Name of the program running within the child process. */
    char* programName;
    /* Status of the child process, as used within the report command.  */
    char* status;
    /* File descriptor for the pipe used to write from the parent to this child
     * process. */
    int pToC;
    /* File descriptor for the pipe used to read from this child within its
     * parent. */
    int cToP;
    /* File stream of the pipe used to read from this child within its parent.
     */
    FILE* readStream;
} Child;

/*
 * Structure type to holf all children processes created by spawn.
 */
typedef struct {
    /* The number of children processes this process has spawned. */
    int numChildren;
    /* Array of pointers to the children processes this process has spawned. */
    Child** children;
} ChildList;

/*
 * Returns the child process specified by the given job ID.
 *
 * If no child has the given job ID, NULL is returned.
 */
Child* get_child_by_jobid(int jobId);

/*
 * Returns the child process specified by the given process ID.
 *
 * If no child has the process job ID, NULL is returned.
 */
Child* get_child_by_pid(int processId);

/*
 * Returns a malloc()'d pointer to an empty child list with count
 * numChildren = 0.
 *
 * The caller of this function is responsible for freeing the memory allocated
 * for the child list and all its children.
 */
ChildList* init_child_list();

/*
 * Creates and returns a new Child struct with the given process ID, job ID,
 * program name and pipe file descriptors and adds it to the given ChildList.
 *
 * The caller of this function is responsible for freeing the memory allocated
 * for the child.
 */
Child* init_child(pid_t processId, char* programName, int pToC, int cToP);

/*
 * Waits on the given child, reaping it and urassigning the appropriate string
 * to the child's status member for use in report().
 *
 * If the status string has been changed from the initial "running", the child
 * must have been waited on after being either exited or signalled and hence is
 * not waited on again.
 */
void wait_on_child(Child* child);

/*
 * Reaps the child which initiated the SIGCHLD signal which caused this
 * function call. Handler function only.
 */
void reap_child(int signum, siginfo_t* info, void* ucontext);

/*
 * Frees the given ChildList and all of its children.
 */
void free_child_list();


#endif
