#include <package/package.h>

t_package *create_empty_package(int op_code)
{
	t_package *package = malloc(sizeof(t_package));
	package->op_code = op_code;
	package->size = 0;
	package->buffer = NULL;
	return package;
}

t_package *create_void_package(int op_code, int block_size, void *message)
{
	t_package *package = create_empty_package(op_code);
	package->size = block_size;
	package->buffer = message;
	return package;
}

t_package *create_string_package(int op_code, char *message)
{
	t_package *package = create_empty_package(op_code);
	package->size = strlen(message) + 1;
	package->buffer = message;
	return package;
}

t_package *create_integer_package(int op_code, int message)
{
	t_package *package = create_empty_package(op_code);
	package->size = sizeof(int);
	package->buffer = malloc(sizeof(int));
	memcpy(package->buffer, &message, sizeof(int));
	return package;
}

t_package *create_uint32_package(int op_code, uint32_t message)
{
	t_package *package = create_empty_package(op_code);
	package->size = sizeof(uint32_t);
	package->buffer = malloc(sizeof(uint32_t));
	memcpy(package->buffer, &message, sizeof(uint32_t));
	return package;
}

void *serialize_list(t_list *list)
{
	int size_list = list_size(list);
	int total_size = sizeof(int) + size_list * sizeof(int);
	void *buffer = malloc(total_size);
	memcpy(buffer, &size_list, sizeof(int));
	int offset = sizeof(int);
	for (int i = 0; i < size_list; i++)
	{
		int block_index = list_get(list, i);
		memcpy(buffer + offset, &block_index, sizeof(int));
		offset += sizeof(int);
	}

	list_destroy(list);

	return buffer;
}

t_list *deserialize_list(void *serialized_data)
{
	int list_size;
	memcpy(&list_size, serialized_data, sizeof(int));
	t_list *result = list_create();
	int offset = sizeof(int);
	for (int i = 0; i < list_size; i++)
	{
		int *block_index_ptr = malloc(sizeof(int));
		memcpy(block_index_ptr, serialized_data + offset, sizeof(int));
		list_add(result, block_index_ptr);
		offset += sizeof(int);
	}
	free(serialized_data);
	return result;
}

void *serialize_package(t_package *package, int bytes)
{
	void *serde_pkg = malloc(bytes);
	int desplazamiento = 0;

	memcpy(serde_pkg + desplazamiento, &(package->op_code), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(serde_pkg + desplazamiento, &(package->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(serde_pkg + desplazamiento, package->buffer, package->size);
	desplazamiento += package->size;

	return serde_pkg;
}

void send_list(int op_code, t_list *list, int client_socket, t_log *logger)
{
	t_package *package = create_empty_package(op_code);
	// Size lista + elementos lista
	package->size = sizeof(int) + list_size(list) * sizeof(int);
	package->buffer = serialize_list(list);
	send_package(package, client_socket, logger);
}

void send_package(t_package *package, int client_socket, t_log *logger)
{
	int bytes = package->size + 2 * sizeof(int);
	void *a_enviar = serialize_package(package, bytes);
	if (send(client_socket, a_enviar, bytes, 0) == -1)
	{
		log_error(logger, "Error al enviar el paquete");
		abort();
	}
	free(a_enviar);
	destroy_package(package);
}

void destroy_package(t_package *package)
{
	free(package->buffer);
	free(package);
}

int receive_op_code(int cliente, t_log *logger)
{
	int op_code;
	if (check_recv(recv(cliente, &op_code, sizeof(int), MSG_WAITALL), logger) != 0)
		return -1;
	return op_code;
}

void *receive_buffer(int client_socket, t_log *logger)
{
	int size;
	void *buffer;
	if (check_recv(recv(client_socket, &size, sizeof(int), MSG_WAITALL), logger) != 0)
		return NULL;
	buffer = malloc(size);
	if (check_recv(recv(client_socket, buffer, size, MSG_WAITALL), logger) != 0)
		return NULL;
	return buffer;
}

t_list *receive_list(int client_socket, t_log *logger)
{
	void *buffer = receive_buffer(client_socket, logger);
	return deserialize_list(buffer);
}