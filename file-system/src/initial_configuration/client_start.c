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

	int send_handshake_result = send_handshake(memory_socket, FILESYSTEM, utils->logger);
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
