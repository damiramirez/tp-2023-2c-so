#ifndef SRC_INITIAL_CONFIGURATION_SERVER_START_H_
#define SRC_INITIAL_CONFIGURATION_SERVER_START_H_

#include <handshake/handshake.h>
#include <connection/connection.h>
#include <config/config.h>

#define NUMERO_DE_PORTS 2

typedef struct {
	int dispatch_fd;
	int interrupt_fd;
} t_conn;

t_conn* start_server_ports(t_utils* utils);

#endif /* SRC_INITIAL_CONFIGURATION_SERVER_START_H_ */
