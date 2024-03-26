#ifndef SRC_CONNECTION_CONNECTION_H_
#define SRC_CONNECTION_CONNECTION_H_

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <commons/log.h>

enum MEMORY_COMMAND
{
    ECHO_MEMORY = 1,
};
enum FILESYSTEM_COMMAND
{
    ECHO_FILESYSTEM = 1,
};
enum CPU_COMMAND
{
    ECHO_CPU_DISPATCH = 1,
    ECHO_CPU_INTERRUPT = 2
};

int connect_to_server(char *ip, char *puerto, t_log *logger);
int start_server(char *port, t_log *logger);
int wait_for_client(int server_fd, t_log *logger);
int check_recv(int result, t_log *logger);

#endif /* SRC_CONNECTION_CONNECTION_H_ */
