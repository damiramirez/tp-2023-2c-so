#include <initial_configuration/server_start.h>

t_conn* start_server_ports(t_utils* utils) {
	t_conn* conn = malloc(sizeof(t_conn));
	if (!check_if_config_value_exists(utils, "PUERTO_ESCUCHA")) {
		utils_destroy(utils);
		free(conn);
		return NULL;
	}

	int server_fd = start_server(config_get_string_value(utils->config, "PUERTO_ESCUCHA"), utils->logger);
	if (server_fd == -1) {
		utils_destroy(utils);
		free(conn);
		return NULL;
	}
	log_info(utils->logger, "Se inicio correctamente");

	int connected_modules;
	for (connected_modules = 0; connected_modules < MODULOS_A_CONECTAR; connected_modules++) {
		int socket_cliente = wait_for_client(server_fd, utils->logger);
		if (socket_cliente == -1) break;
		int handshake = receive_handshake(socket_cliente, utils->logger);
		if (handshake == -1) break;
		switch(handshake) {
			case FILESYSTEM:
				conn->socket_filesystem = socket_cliente;
				send_handshake_success(conn->socket_filesystem, utils->logger);
				log_info(utils->logger, "Se conecto el FileSystem");
				break;
			case CPU:
				conn->socket_cpu = socket_cliente;
				send_handshake_success(conn->socket_cpu, utils->logger);
				log_info(utils->logger, "Se conecto la CPU");
				break;
			case KERNEL:
				conn->socket_kernel = socket_cliente;
				send_handshake_success(conn->socket_kernel, utils->logger);
				log_info(utils->logger, "Se conecto el Kernel");
				break;
			default:
				send_handshake_error(socket_cliente, utils->logger);
				close(socket_cliente);
				break;
		}
	}
	if (connected_modules < MODULOS_A_CONECTAR) {
		utils_destroy(utils);
		free(conn);
		return NULL;
	}
	return conn;
}