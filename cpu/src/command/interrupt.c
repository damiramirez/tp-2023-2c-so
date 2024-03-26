#include <command/interrupt.h>

int wait_for_interrupt_echo(t_utils* utils, t_conn* conn) {
    int op_code = receive_op_code(conn->interrupt_fd, utils->logger);
	if (op_code == -1) return -1;
    if (op_code != ECHO_CPU_INTERRUPT) {
		log_error(utils->logger, "Unknown OpCode");
		return -1;
    }
    char* message = receive_buffer(conn->interrupt_fd, utils->logger);
	log_info(utils->logger, "OpCode: %d and Message: %s", op_code, message);
	free(message);
    return 0;
}