#ifndef SRC_INITIAL_CONFIGURATION_CLIENT_START_H_
#define SRC_INITIAL_CONFIGURATION_CLIENT_START_H_

#include <handshake/handshake.h>
#include <connection/connection.h>
#include <config/config.h>
#include <package/package.h>

int connect_to_memory(t_utils* utils);
int* get_page_size_of_memory(int memory_socket, t_utils* utils);

#endif /* SRC_INITIAL_CONFIGURATION_CLIENT_START_H_ */

