#include <stdlib.h>
#include <stdio.h>
#include <initial_configuration/client_start.h>
#include <initial_configuration/server_start.h>
#include <initial_configuration/fs_config.h>
#include <command/command.h>
#include "fcb/fcb.h"
#include "fs-struct/fat-bloque.h"
#include <commons/bitarray.h>
#include <commons/memory.h>

#define LOGS_FILE_SYSTEM "filesystem.log"

t_utils *utils;
t_list *fcbs;

extern t_list *fat_list;

int main(int argc, char *argv[])
{
	utils = create_initial_config(argc, argv, LOGS_FILE_SYSTEM, true, argv[2]);
	if (utils == NULL)
		return EXIT_FAILURE;

	fs_config = malloc(sizeof(t_fs_config));
	init_fs_config(utils->config, fs_config);

	fcbs = list_create();
	load_FCBs_from_directory(fs_config->path_fcb);
	init_fs_struct();

	int memory_socket = connect_to_memory(utils);
	if (memory_socket == -1)
		return EXIT_FAILURE;

	log_debug(utils->logger, "Se conecto a memory");

	int socket_kernel = start_server_port(utils);
	if (socket_kernel == -1)
	{
		close(memory_socket);
		return EXIT_FAILURE;
	}

	wait_in_every_port(memory_socket, socket_kernel, utils->logger);

	utils_destroy_with_connection(utils, memory_socket);
	
	void _destroy_fcbs(t_fcb* fcb) {
		free(fcb->file_name);
		free(fcb);
	};
	list_destroy_and_destroy_elements(fcbs, _destroy_fcbs);
	destroy_fs();
	destroy_fs_config(fs_config);

	return EXIT_SUCCESS;
}
