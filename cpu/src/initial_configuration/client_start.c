#include <initial_configuration/client_start.h>

int connect_to_memory(t_utils* utils) {
	if (!check_if_config_value_exists(utils, "PUERTO_MEMORIA") || !check_if_config_value_exists(utils, "IP_MEMORIA")) {
		utils_destroy(utils);
		return -1;
	}
	char* ip = config_get_string_value(utils->config, "IP_MEMORIA");
	char* port = config_get_string_value(utils->config, "PUERTO_MEMORIA");
	int memory_socket = connect_to_server(ip, port, utils->logger);
	if (memory_socket == -1) {
		utils_destroy(utils);
		return -1;
	}

	int send_handshake_result = send_handshake(memory_socket, CPU, utils->logger);
	if (send_handshake_result == -1) {
		utils_destroy_with_connection(utils, memory_socket);
		return -1;
	}
	int handshake_result = check_handshake_result(memory_socket, utils->logger);
	if (handshake_result == -1) {
		utils_destroy_with_connection(utils, memory_socket);
		return -1;
	}
	return memory_socket;
}

int* get_page_size_of_memory(int memory_socket, t_utils* utils) {
	const int page_size_command = 4;
	if (send(memory_socket, &page_size_command, sizeof(int), 0) == -1) {
		log_error(utils->logger, "Error al enviar el comando");
		utils_destroy_with_connection(utils, memory_socket);
		return NULL;
	}
	int op_code = receive_op_code(memory_socket, utils->logger);
	if (op_code != page_size_command) {
		utils_destroy_with_connection(utils, memory_socket);
		return NULL;
	}
	int* page_size = receive_buffer(memory_socket, utils->logger);
	if (page_size == NULL) {
		utils_destroy_with_connection(utils, memory_socket);
		return NULL;
	}
	log_trace(utils->logger, "Page Size: %d", *page_size);
	return page_size;
}