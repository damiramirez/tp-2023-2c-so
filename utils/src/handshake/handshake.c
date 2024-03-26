#include <handshake/handshake.h>

int send_handshake(int conexion, int handshake, t_log* logger) {
	int send_result = send(conexion, &handshake, sizeof(int), 0);
	if (send_result == -1) log_error(logger, "Error al enviar el handshake");
	return send_result;
}

int send_handshake_success(int cliente, t_log* logger) {
	const int success = 0;
	int send_result = send(cliente, &success, sizeof(int), 0);
	if (send_result == -1) log_error(logger, "Error al enviar success en el handshake");
	return send_result;
}

int send_handshake_error(int cliente, t_log* logger) {
	const int error = -1;
	int send_result = send(cliente, &error, sizeof(int), 0);
	if (send_result == -1) log_error(logger, "Error al enviar error en el handshake");
	return send_result;
}

int receive_handshake(int cliente, t_log* logger) {
	int handshake;
	int receive_result = recv(cliente, &handshake, sizeof(int), MSG_WAITALL);
	if (receive_result == -1) {
		log_error(logger, "Error al recibir el handshake");
		return -1;
	}
	return handshake;
}

int check_handshake_result(int servidor, t_log* logger) {
	int handshake_result;
	int receive_result = recv(servidor, &handshake_result, sizeof(int), MSG_WAITALL);
	if (receive_result == -1) {
		log_error(logger, "Error al recibir el resultado del handshake");
		return receive_result;
	}
	if (handshake_result == 0) {
		log_info(logger, "Hizo el handshake correctamente");
		return receive_result;
	} else {
		log_error(logger, "Fallo en el handshake");
		close(servidor);
		return -1;
	}
}

int wait_for_initial_handshake_from_kernel(int server_fd, t_log* logger) {
	int socket_kernel = wait_for_client(server_fd, logger);
	int handshake = receive_handshake(socket_kernel, logger);
	if (handshake == -1) return handshake;
	if (handshake == KERNEL) {
		send_handshake_success(socket_kernel, logger);
		log_info(logger, "Se conecto al Kernel");
	} else {
		send_handshake_error(socket_kernel, logger);
		close(socket_kernel);
	}
	return socket_kernel;
}
