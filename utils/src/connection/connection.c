#include <connection/connection.h>

int connect_to_server(char *ip, char* puerto, t_log* logger)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente;
	socket_cliente = socket(server_info->ai_family,
			server_info->ai_socktype,
			server_info->ai_protocol);

	int conexion = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
	freeaddrinfo(server_info);
	if (conexion != 0) {
		log_error(logger, "Fallo en la conexion al servidor");
		return -1;
	}
	log_info(logger, "Conecto correctamente al servidor");
	return socket_cliente;
}

int start_server(char* port, t_log* logger)
{
	int socket_servidor;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, port, &hints, &servinfo);

	socket_servidor = socket(servinfo->ai_family,
	                         servinfo->ai_socktype,
	                         servinfo->ai_protocol);
	if (socket_servidor == -1) {
		log_error(logger, "Fallo al crear el servidor");
		return -1;
	}

	int bind_result = bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	if (bind_result != 0) {
		log_error(logger, "Fallo al hacer el bind en el servidor");
		return -1;
	}
	int listen_result = listen(socket_servidor, SOMAXCONN);
	if (listen_result != 0) {
		log_error(logger, "Fallo al hacer el listen en el servidor");
		return -1;
	}
	freeaddrinfo(servinfo);
	return socket_servidor;
}

int wait_for_client(int server_fd, t_log* logger) {
	int socket_cliente = accept(server_fd, NULL, NULL);
	if (socket_cliente == -1) log_error(logger, "Error al aceptar un cliente");
	return socket_cliente;
}

int check_recv(int result, t_log* logger) {
	if (result == 0) {
		log_info(logger, "Se corto la conexion con el cliente");
		return -1;
	}
	if (result == -1) {
		log_error(logger, "Error en el recv");
		return -1;
	}
	return 0;
}