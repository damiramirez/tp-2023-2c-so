#include <initial_configuration/server_start.h>

t_conn* start_server_ports(t_utils* utils) {
	t_conn* conn = malloc(sizeof(t_conn));
	char* port_keys[] = { "PUERTO_ESCUCHA_DISPATCH", "PUERTO_ESCUCHA_INTERRUPT" };
	for (int i = 0; i < NUMERO_DE_PORTS; i++) {
		char* port_string = port_keys[i];
		if (!check_if_config_value_exists(utils, port_string)) {
			utils_destroy(utils);
			free(conn);
			return NULL;
		}
		int fd = start_server(config_get_string_value(utils->config, port_string), utils->logger);
		switch (i) {
			case 0:
				conn->dispatch_fd = fd;
				log_info(utils->logger, "Se inicio correctamente el server de dispatch");
			break;
			case 1:
				conn->interrupt_fd = fd;
				log_info(utils->logger, "Se inicio correctamente el server de interrupt");
			break;
		}
	}

	int socket_dispatch_fd = wait_for_initial_handshake_from_kernel(conn->dispatch_fd, utils->logger);
	if (socket_dispatch_fd == -1) {
		utils_destroy(utils);
		free(conn);
		return NULL;
	}
	conn->dispatch_fd = socket_dispatch_fd;
	int socket_interrupt_fd = wait_for_initial_handshake_from_kernel(conn->interrupt_fd, utils->logger);
	if (socket_interrupt_fd == -1) {
		utils_destroy(utils);
		free(conn);
		return NULL;
	}
	conn->interrupt_fd = socket_interrupt_fd;
	return conn;
}
