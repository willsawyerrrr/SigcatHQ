/**
 * Sets the handler to ignore the interrupt signal (SIGINT).
 */
void ignoreInterrupt();

/**
 * Parse the given command strintg.
 */
void parse(char* command);

void spawn(int numArgs, char** args, char* command);

void report(int numArgs, char** args, char* command);

void send_signal(int numArgs, char** args, char* command);

void sleep(int numArgs, char** args, char* command);

void send(int numArgs, char** args, char* command);

void rcv(int numArgs, char** args, char* command);

void eof(int numArgs, char** args, char* command);

void cleanup();

int validate_num_args(int minExpected, int given);

int validate_job_id(int jobId);

