#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "initial_configuration/client_start.h"
#include "initial_configuration/server_start.h"
#include <command/dispatch.h>

#define LOGS_CPU "cpu.log"

int main(int argc, char* argv[]) {
	t_utils* utils = create_initial_config(argc, argv, LOGS_CPU, true, argv[2]);
	if (utils == NULL) return EXIT_FAILURE;

	int memory_socket = connect_to_memory(utils);
	if (memory_socket == -1) return EXIT_FAILURE;

	t_conn* ports = start_server_ports(utils);
	if (ports == NULL) {
		close(memory_socket);
		return EXIT_FAILURE;
	}

	int* page_size = get_page_size_of_memory(memory_socket, utils);
	if (page_size == NULL) {
		free(ports);
		free(page_size);
		return EXIT_FAILURE;
	}

	t_reg registers;
	registers.AX = 0;
	registers.BX = 0;
	registers.CX = 0;
	registers.DX = 0;
	int commands_result = wait_for_dispatch_command(utils, ports, memory_socket, &registers, *page_size);

	utils_destroy_with_connection(utils, memory_socket);
	free(ports);
	free(page_size);
    return commands_result == -1 ? EXIT_FAILURE : EXIT_SUCCESS;
}
