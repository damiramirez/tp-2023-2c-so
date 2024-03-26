#include <initial_configuration/client_start.h>

t_conn* connect_to_modules(t_utils* utils) {
	t_conn* conn = malloc(sizeof(t_conn));
	char* connection_config[] = {
			"PUERTO_MEMORIA", "IP_MEMORIA",
			"PUERTO_FILESYSTEM", "IP_FILESYSTEM",
			"PUERTO_CPU_DISPATCH", "IP_CPU",
			"PUERTO_CPU_INTERRUPT", "IP_CPU"
	};

	int connected_modules;
	for (connected_modules = 0; connected_modules < NUMERO_DE_CONNECTION_KEYS; connected_modules = connected_modules + 2) {
		char* port_key = connection_config[connected_modules];
		char* ip_key = connection_config[connected_modules + 1];
		if (!check_if_config_value_exists(utils, port_key) || !check_if_config_value_exists(utils, ip_key)) break;
		char* port = config_get_string_value(utils->config, port_key);
		char* ip = config_get_string_value(utils->config, ip_key);
		int conexion = connect_to_server(ip, port, utils->logger);
		if (conexion == -1) break;
		int handshake_send_result = send_handshake(conexion, KERNEL, utils->logger);
		if (handshake_send_result == -1) break;
		int handshake_result = check_handshake_result(conexion, utils->logger);
		if (handshake_result == -1) break;
		switch (connected_modules) {
			case 0:
				conn->memory_socket = conexion;
				log_info(utils->logger, "Conectado a Memory");
			break;
			case 2:
				conn->filesystem_socket = conexion;
				log_info(utils->logger, "Conectado al FileSystem");
			break;
			case 4:
				conn->cpu_dispatcher_socket = conexion;
				log_info(utils->logger, "Conectado al Dispatcher de la CPU");
			break;
			case 6:
				conn->cpu_interrupt_socket = conexion;
				log_info(utils->logger, "Conectado al Interrupt de la CPU");
			break;
		}
	}
	if (connected_modules != NUMERO_DE_CONNECTION_KEYS) {
		log_error(utils->logger, "Error in module %d", connected_modules);
		if (connected_modules >= 2) close(conn->memory_socket);
		if (connected_modules >= 4) close(conn->filesystem_socket);
		if (connected_modules >= 6) close(conn->cpu_dispatcher_socket);
		utils_destroy(utils);
		free(conn);
	    return NULL;
	}

	return conn;
}

void close_conn(t_conn* conn) {
	close(conn->memory_socket);
	close(conn->filesystem_socket);
	close(conn->cpu_dispatcher_socket);
	close(conn->cpu_interrupt_socket);
	free(conn);
}
