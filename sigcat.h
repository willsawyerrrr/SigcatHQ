/**
 * Sets the handler for all signals (1 to 31) to the handler method.
 */
void set_handlers();

/**
 * Print the following text:
 *     sigcat received <signal name>
 * to the current output stream, where <signal name> is replaced by
 * the signal name reported by strsignal().
 */
void handler(int signum);

