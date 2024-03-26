#ifndef SRC_PACKAGE_PACKAGE_H_
#define SRC_PACKAGE_PACKAGE_H_

#include <stdlib.h>
#include <string.h>
#include <handshake/handshake.h>
#include <commons/collections/list.h>

typedef struct
{
    int op_code;
    int size;
    void *buffer;
} t_package;

t_package *create_empty_package(int op_code);
t_package *create_string_package(int op_code, char *message);
t_package *create_integer_package(int op_code, int message);
t_package *create_uint32_package(int op_code, uint32_t message);
t_package *create_void_package(int op_code, int block_size, void *message);
void send_package(t_package *package, int client_socket, t_log *logger);
void destroy_package(t_package *paquete);
void *serialize_package(t_package *package, int bytes);
void *serialize_list(t_list *list);
void send_list(int op_code, t_list *list, int client_socket, t_log *logger);
t_list *deserialize_list(void *serialized_data);
int receive_op_code(int cliente, t_log *logger);
void *receive_buffer(int client_socket, t_log *logger);
t_list *receive_list(int client_socket, t_log *logger);

#endif /* SRC_PACKAGE_PACKAGE_H_ */