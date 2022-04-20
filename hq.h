#ifndef HQ_H
#define HQ_H

#include <sys/types.h>

/*
 * Structure type to hold a child process - created within spawn.
 */
typedef struct {
    /* Process ID of the newly created child, relative to the kernel. */
    int processId;
    /* Job ID of the newly created child, relative to hq. */
    pid_t jobId;
    /* Name of the program running within the child process. */
    char* programName;
    /* File descriptor for the pipe used to write from the parent (this hq
     * process) to this child process. */
    int pToC;
    /* File descriptor for the pipe used to read from this child within its
     * parent (this hq process).
     */
    int cToP;
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
 * Sets a handler to ignore the interrupt signal.
 */
void ignore_interrupt();

/*
 * Parses the given command string. If the first argument of the command string
 * is the name of a command, that command is called.
 */
void parse(char* command, ChildList* childList);

/*
 * Usage: spawn <program> [<arg1>] [<arg2>] ...
 *
 * Runs the given program in a new process, with the arguments provided, if
 * any. Arguments or program names containing spacesmay be quoted in double
 * quotes. Standard input to and output from the new proceed can be accessed
 * with the send and rcv commands, respectively. The new process's standard
 * error is unchanged.
 */
void spawn(int numArgs, char** args, ChildList* childList);

/*
 * Determines whether the given command string is valid to execute the spawn
 * command.
 *
 * The command string is valid if and only if:
 *  - <program> is present.
 *
 * All extraneous arguments are ignored.
 *
 * Returns 1 (true) if the command string is valid; 0 (false) otherwise.
 */
int validate_spawn_args(int numArgs, char** args, ChildList* childList);

/*
 * Usage: report [<jobid>]
 *
 * Reports on the status of the job with the given job ID or all jobs if the
 * jobid parameter is not provided.
 */
void report(int numArgs, char** args, ChildList* childList);

/*
 * Determines whether the given command string is valid to execute the report
 * command.
 *
 * The command string is valid if and only if:
 *  - <jobid> is either not present or is a complete and valid integer
 *    corresponding to the job ID of a process created using the spawn command.
 *
 * All extraneous arguments are ignored.
 *
 * Returns 1 (true) if the command string is valid; 0 (false) otherwise.
 */
int validate_report_args(int numArgs, char** args, ChildList* childList);

/*
 * Usage: signal <jobid> <signum>
 *
 * Send the signal with the given signum to the job with the given job ID.
 */
void send_signal(int numArgs, char** args, ChildList* childList);

/*
 * Determines whether the given command string is valid to execute the signal
 * command.
 *
 * The command string is valid if and only if:
 *  - <jobid> is a complete and valid integer corresponding to a process
 *    created using the spawn command; and
 *  - <signum> is a complete and valid integer between 1 and 31, inclusive.
 *
 * All extraneous arguments are ignored.
 *
 * Returns 1 (true) if the command string is valid; 0 (false) otherwise.
 */
int validate_signal_args(int numArgs, char** args, ChildList* childList);

/*
 * Usage: sleep <seconds>
 *
 * Causes this process to sleep for the given number of seconds. The specified
 * number of seconds can be integral or fractional.
 */
void sleep_hq(int numArgs, char** args, ChildList* childList);

/*
 * Determines whether the given command string is valid to execute the sleep
 * command.
 *
 * The command string is valid if and only if:
 *  - <seconds> is a complete and valid non-negative number.
 *
 * All extraneous arguments are ignored.
 *
 * Returns 1 (true) if the command string is valid; 0 (false) otherwise.
 */
int validate_sleep_args(int numArgs, char** args, ChildList* childList);

/*
 * Usage: send <jobid> <text>
 *
 * Sends the given text to the job with the given job ID. Strings containing
 * spaces must be quoted in double quotes.
 */
void send(int numArgs, char** args, ChildList* childList);

/*
 * Determines whether the given command string is valid to execute the send
 * command.
 *
 * The command string is valid if and only if:
 *  - <jobid> is a complete and valid integer corresponding to the job ID of a
 *    process created using the spawn command; and
 * - <text> is present.
 *
 * All extraneous arguments are ignored.
 *
 * Returns 1 (true) if the command string is valid; 0 (false) otherwise.
 */
int validate_send_args(int numArgs, char** args, ChildList* childList);

/*
 * Usage: rcv <jobid>
 *
 * Attempts to read one line of text from the job with the given job ID and
 * displays it to this process's standard out.
 */
void rcv(int numArgs, char** args, ChildList* childList);

/*
 * Determines whether the given command string is valid to execute the rcv
 * command.
 *
 * The command string is valid if and only if:
 *  - <jobid> is a complete and valid integer corresponding to the job ID of a
 *    process created using the spawn command.
 *
 * All extraneous arguments are ignored.
 *
 * Returns 1 (true) if the command string is valid; 0 (false) otherwise.
 */
int validate_rcv_args(int numArgs, char** args, ChildList* childList);

/*
 * Usage: eof <jobid>
 *
 * Closes the pipe connected to the standard input of the job with the given
 * job ID, causing it to receive EOF on its next read attempt.
 */
void eof(int numArgs, char** args, ChildList* childList);

/*
 * Determines whether the given command string is valid to execute the eof
 * command.
 *
 * The command string is valid if and only if:
 *  - <jobid> is a complete and valid integer corresponding to the job ID of a
 *    process created using the spawn command.
 *
 * All extraneous arguments are ignored.
 *
 * Returns 1 (true) if the command string is valid; 0 (false) otherwise.
 */
int validate_eof_args(int numArgs, char** args, ChildList* childList);

/*
 * Usage: cleanup()
 *
 * Terminates and reaps all child processes spawned by this process by sending
 * them the kill signal.
 */
void cleanup();

/*
 * Determines whether the given number of arguments is valid, given the
 * expected number of arguments.
 *
 * Returns 1 (true) if and only if given >= minExpected; 0 (false) otherwise.
 */
int validate_num_args(int minExpected, int given);

/*
 * Determines whether the given job ID is valid.
 *
 * Returns 1 (true) if and only if jobId is the id of a child of this process;
 * 0 (false) otherwise.
 */
int validate_job_id(int jobId, ChildList* childList);

/**
 * Returns the child process specified by the given job ID.
 *
 * If no child has the given job ID, NULL is returned.
 */
Child* get_child_by_job_id(int jobId, ChildList* childList);

/**
 * Returns a malloc()'d pointer to an empty child list with count
 * numChildren = 0.
 *
 * The caller of this function is responsible for freeing the memory allocated
 * for the child list and all its children.
 */
ChildList* init_child_list();

void free_child_list(ChildList* childList);

#endif
