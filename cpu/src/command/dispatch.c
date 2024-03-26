#include <command/dispatch.h>

int wait_for_dispatch_command(t_utils* utils, t_conn* ports, int memory_socket, t_reg* registers, int page_size) {
    int op_code = receive_op_code(ports->dispatch_fd, utils->logger);
	if (op_code == -1) return -1;
	while (op_code)
	{
		switch (op_code) {
			case ECHO_CPU_DISPATCH:
				char* message = receive_buffer(ports->dispatch_fd, utils->logger);
				log_info(utils->logger, "OpCode: %d and Message: %s", op_code, message);
				free(message);
                int interrupt_echo_result = wait_for_interrupt_echo(utils, ports);
                if (interrupt_echo_result == -1) return -1;
				log_info(utils->logger, "ECHO to Memory");
				t_package* package = create_string_package(ECHO_MEMORY, "ECHO To Memory from CPU");
				send_package(package, memory_socket, utils->logger);
				break;
			case INSTRUCTION:
				t_pcb* pcb = receive_pcb(ports->dispatch_fd, utils->logger);
				bool continue_executing = true;
				while (continue_executing == true)
				{
					char* instruction = fetch(pcb, memory_socket, utils->logger);
					if (instruction == NULL) {
						return -1;
					} 
					t_ins formatted_instruction = decode(pcb, instruction, page_size, utils->logger, memory_socket);
					if (pcb->instruccion == PAGE_FAULT) {
						log_info(utils->logger, "Page Fault PID: %d - Pagina: %d", pcb->pid, pcb->params);
						destroy_instruction(formatted_instruction);
						continue_executing = false;
					} else {
						int execute_result = execute(pcb, ports, memory_socket, registers, formatted_instruction, utils->logger);
						if (execute_result != RETURN_CONTEXT) {
							if (execute_result == CONTINUE) {
								pcb->programCounter++;
							}
							int check = check_interrupt(pcb, ports->interrupt_fd, utils->logger);
							switch (check)
							{
								case FAIL_CONNECTION:
									destroy_pcb(pcb);
									return -1;						
								case INTERRUPTION:
									continue_executing = false;
									break;
								case NO_INTERRUPTION:
									continue_executing = true;
									break;
							}
						} else {
							continue_executing = false;
						}
					}
				}
				pcb->registers = *registers;
				send_pcb(EXECUTED_INSTRUCTION, pcb, ports->dispatch_fd, utils->logger);
				destroy_pcb(pcb);
				break;
			default:
				log_error(utils->logger, "Unknown OpCode %d", op_code);
				return -1;
		}
		op_code = receive_op_code(ports->dispatch_fd, utils->logger);
	    if (op_code == -1) return -1;
	}
    return 0;
}