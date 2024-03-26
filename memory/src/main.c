#include <stdlib.h>
#include <stdio.h>
#include <config/config.h>
#include <command/command.h>
#include <initial_configuration/memory_config.h>
#include <user_memory/paging/algorithms.h>
#include <user_memory/paging/pages.h>
#include <instruction_memory/process/process.h>
#include <user_memory/real_memory.h>

#define LOGS_MEMORIA "memoria.log"

int main(int argc, char* argv[]) {
	t_utils* utils = create_initial_config(argc, argv, LOGS_MEMORIA, true, argv[2]);
	if (utils == NULL) return EXIT_FAILURE;

	t_conn* conn = start_server_ports(utils);
	if (conn == NULL) return EXIT_FAILURE;

	active_processes = list_create();

	init_memory_config(utils->config);
	init_real_memory();
	init_sorter_thread();
	
	send_page_size_to_cpu(conn, utils);

	wait_in_every_port(conn, utils->logger);

	log_info(utils->logger, "Se termina el programa");
	free_memory(utils->logger);
	free(conn);
	utils_destroy(utils);
	
    return EXIT_SUCCESS;
}