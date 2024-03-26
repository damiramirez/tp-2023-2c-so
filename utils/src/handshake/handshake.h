#ifndef SRC_HANDSHAKE_HANDSHAKE_H_
#define SRC_HANDSHAKE_HANDSHAKE_H_

#include <stdlib.h>
#include <sys/socket.h>
#include <commons/log.h>
#include <unistd.h>
#include <connection/connection.h>

enum MODULOS {
	FILESYSTEM = 1,
	CPU = 2,
	MEMORY = 3,
	KERNEL = 4
};

int receive_handshake(int cliente, t_log* logger);
int send_handshake(int conexion, int handshake, t_log* logger);
int send_handshake_success(int cliente, t_log* logger);
int send_handshake_error(int cliente, t_log* logger);
int check_handshake_result(int servidor, t_log* logger);
int wait_for_initial_handshake_from_kernel(int server_fd, t_log* logger);

#endif /* SRC_HANDSHAKE_HANDSHAKE_H_ */
