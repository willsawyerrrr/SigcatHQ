#ifndef HQ_H
#define HQ_H

#include "child.h"

#include <signal.h>
#include <sys/types.h>

/*
 * Sets handlers to ignore the interrupt and broken pipe signals and reap
 * child process upon receiving the child signal.
 */
void set_handlers();

/*
 * Parses the given command string. If the first argument of the command string
 * is the name of a command, that command is called.
 */
void parse(char* command);

/*
 * Usage: spawn <program> [<arg1>] [<arg2>] ...
 *
 * Runs the given program in a new process, with the arguments provided, if
 * any. Arguments or program names containing spacesmay be quoted in double
 * quotes. Standard input to and output from the new proceed can be accessed
 * with the send and rcv commands, respectively. The new process's standard
 * error is unchanged.
 */
void spawn(int numArgs, char** args);

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
int validate_spawn_args(int numArgs, char** args);

/*
 * Usage: report [<jobid>]
 *
 * Reports on the status of the job with the given job ID or all jobs if the
 * jobid parameter is not provided.
 */
void report(int numArgs, char** args);

/*
 * Reports on the given child process's status. The format of the resultant
 * string is:
 *      [Job] cmd:status
 * Where Job is the jobId of the process, cmd is the name of the program the
 * process is executing and status is the status of the process and is either:
 *      running             ;
 *      exited(code)        , where code is the exit code; or
 *      signalled(signum)   , where signum is the signal number.
 */
void report_single(Child* child);

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
int validate_report_args(int numArgs, char** args);

/*
 * Usage: signal <jobid> <signum>
 *
 * Send the signal with the given signum to the job with the given job ID.
 */
void send_signal(int numArgs, char** args);

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
int validate_signal_args(int numArgs, char** args);

/*
 * Usage: sleep <seconds>
 *
 * Causes this process to sleep for the given number of seconds. The specified
 * number of seconds can be integral or fractional.
 */
void sleep_hq(int numArgs, char** args);

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
int validate_sleep_args(int numArgs, char** args);

/*
 * Usage: send <jobid> <text>
 *
 * Sends the given text to the job with the given job ID. Strings containing
 * spaces must be quoted in double quotes.
 */
void send(int numArgs, char** args);

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
int validate_send_args(int numArgs, char** args);

/*
 * Usage: rcv <jobid>
 *
 * Attempts to read one line of text from the job with the given job ID and
 * displays it to this process's standard out.
 */
void rcv(int numArgs, char** args);

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
int validate_rcv_args(int numArgs, char** args);

/*
 * Usage: eof <jobid>
 *
 * Closes the pipe connected to the standard input of the job with the given
 * job ID, causing it to receive EOF on its next read attempt.
 */
void eof(int numArgs, char** args);

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
int validate_eof_args(int numArgs, char** args);

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
 * Determines whether the given argument represents a complete and valid
 * number. If allowFractional is set, the given argument may represent a
 * fractional number; otherwise, the argument must represent an integer.
 *
 * Returns 1 (true) if and only if arg is a valid and complete numerical
 * argument; 0 otherwise.
 */
int validate_numerical_arg(char* arg, int allowFractional);

/*
 * Determines whether the given integer is a valid job ID. A valid job ID is
 * one which corresponds to a child in the given ChildList, created using the
 * spawn command.
 *
 * Returns 1 (true) if and only if arg represents the job ID of a child of this
 * process; 0 (false) otherwise.
 */
int validate_jobid(char* jobId);

/*
 * Determines whether the given integer is a valid signal number. A signal
 * number is valid if and only if it is between 1 and 31, inclusive.
 *
 * Returns 1 (true) if and only if arg is a valid signal number; 0 otherwise.
 */
int validate_signum(char* signum);

#endif

