#ifndef SRC_INITIAL_CONFIGURATION_SERVER_START_H_
#define SRC_INITIAL_CONFIGURATION_SERVER_START_H_

#include <connection/connection.h>
#include <handshake/handshake.h>
#include <config/config.h>

#define MODULOS_A_CONECTAR 3

typedef struct {
	int socket_filesystem;
	int socket_cpu;
	int socket_kernel;
} t_conn;

t_conn* start_server_ports(t_utils* utils);

#endif /* SRC_INITIAL_CONFIGURATION_SERVER_START_H_ */