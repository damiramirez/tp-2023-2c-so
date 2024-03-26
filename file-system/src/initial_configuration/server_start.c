#include <initial_configuration/server_start.h>

int start_server_port(t_utils* utils) {
	if (!check_if_config_value_exists(utils, "PUERTO_ESCUCHA")) {
		utils_destroy(utils);
		return -1;
	}
	int server_fd = start_server(config_get_string_value(utils->config, "PUERTO_ESCUCHA"), utils->logger);
	if (server_fd == -1) {
		utils_destroy(utils);
		return -1;
	}
	log_info(utils->logger, "Se inicio correctamente el server");
	int socket_kernel = wait_for_initial_handshake_from_kernel(server_fd, utils->logger);
	if (socket_kernel == -1) {
		utils_destroy(utils);
		return -1;
	}
	return socket_kernel;
}
