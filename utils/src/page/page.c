#include "page.h"

int size_of_page(void)
{
	return sizeof(uint32_t) + 2 * sizeof(int);
}

static int size_of_page_for_mov_out(void)
{
	return 2 * sizeof(uint32_t) + 2 * sizeof(int);
}

static int size_of_page_for_swap(int content_size)
{
	return 2 * sizeof(int) + content_size;
}

static int size_of_page_for_mov_out_fs(int content_size)
{
	return 3 * sizeof(int) + content_size + sizeof(uint32_t);
}

t_pag *page_new(uint32_t pid, int page_number, int displacement)
{
	t_pag *page = malloc(sizeof(*page));
	page->pid = pid;
	page->page_number = page_number;
	page->displacement = displacement;
	return page;
}

t_pag_swap *page_for_swap(int swap_block, int page_info_size, void *page_content)
{
	t_pag_swap *page = malloc(sizeof(*page));
	page->swap_block = swap_block;
	page->page_info_size = page_info_size;
	page->page_content = malloc(page_info_size);
	page->page_content = page_content;
	return page;
}

t_mov_out *page_for_mov_out(uint32_t pid, int page_number, int displacement, uint32_t register_value)
{
	t_mov_out *page = malloc(sizeof(*page));
	page->pid = pid;
	page->page_number = page_number;
	page->displacement = displacement;
	page->register_value = register_value;
	return page;
}

t_mov_out_fs *page_for_mov_out_fs(uint32_t pid, int page_number, int displacement, void *register_value, int size)
{
	t_mov_out_fs *page = malloc(sizeof(*page));
	page->pid = pid;
	page->page_number = page_number;
	page->displacement = displacement;
	page->register_value = register_value;
	page->size = size;
	return page;
}

void destroy_page(t_pag *page)
{
	free(page);
}

void destroy_page_for_mov_out(t_mov_out *page)
{
	free(page);
}

void destroy_page_for_mov_out_fs(t_mov_out_fs *page)
{
	free(page->register_value);
	free(page);
}

void destroy_page_for_swap(t_pag_swap *page)
{
	free(page->page_content);
	free(page);
}

static void *serialize_page_for_mov_out(t_mov_out *page)
{
	void *buffer = malloc(size_of_page_for_mov_out());
	int offset = 0;
	memcpy(buffer + offset, &page->page_number, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer + offset, &page->displacement, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer + offset, &page->pid, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer + offset, &page->register_value, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	return buffer;
}

void *serialize_page_for_mov_out_fs(t_mov_out_fs *page)
{
	void *buffer = malloc(size_of_page_for_mov_out_fs(page->size));
	int offset = 0;
	memcpy(buffer + offset, &page->page_number, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer + offset, &page->displacement, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer + offset, &page->pid, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer + offset, &page->size, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer + offset, page->register_value, page->size);
	offset += page->size;

	return buffer;
}

void *serialize_page(t_pag *page)
{
	void *buffer = malloc(size_of_page());
	int offset = 0;
	memcpy(buffer + offset, &page->page_number, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer + offset, &page->displacement, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer + offset, &page->pid, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	return buffer;
}

void *serialize_page_for_swap(t_pag_swap *page)
{
	void *buffer = malloc(size_of_page_for_swap(page->page_info_size));
	int offset = 0;
	memcpy(buffer + offset, &page->swap_block, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer + offset, &page->page_info_size, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer + offset, page->page_content, page->page_info_size);
	offset += page->page_info_size;
	return buffer;
}

t_pag *deserialize_page(void *buffer)
{
	t_pag *page = malloc(sizeof(*page));
	int offset = 0;
	memcpy(&page->page_number, buffer + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&page->displacement, buffer + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&page->pid, buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	free(buffer);
	return page;
}

static t_mov_out *deserialize_page_for_mov_out(void *buffer)
{
	t_mov_out *page = malloc(sizeof(*page));
	int offset = 0;
	memcpy(&page->page_number, buffer + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&page->displacement, buffer + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&page->pid, buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&page->register_value, buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	free(buffer);
	return page;
}

t_mov_out_fs *deserialize_page_for_mov_out_fs(void *buffer)
{
	t_mov_out_fs *page = malloc(sizeof(*page));
	int offset = 0;
	memcpy(&page->page_number, buffer + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&page->displacement, buffer + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&page->pid, buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&page->size, buffer + offset, sizeof(int));
	offset += sizeof(int);
	int size = page->size + 1; // para que no lea el '\0'
	page->register_value = malloc(size);
	memcpy(page->register_value, buffer + offset, page->size);
	offset += page->size;
	free(buffer);
	return page;
}

t_pag_swap *deserialize_page_for_swap(void *buffer)
{
	t_pag_swap *page = malloc(sizeof(*page));
	int offset = 0;
	memcpy(&page->swap_block, buffer + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&page->page_info_size, buffer + offset, sizeof(int));
	offset += sizeof(int);
	int size = page->page_info_size + 1;
	page->page_content = malloc(size); // para que no lea el '\0'
	memcpy(page->page_content, buffer + offset, page->page_info_size);
	offset += page->page_info_size;
	free(buffer);
	return page;
}

void send_page_for_mov_out(int op_code, t_mov_out *page, int client_socket, t_log *logger)
{
	t_package *package = create_empty_package(op_code);
	package->size = size_of_page_for_mov_out();
	package->buffer = serialize_page_for_mov_out(page);
	send_package(package, client_socket, logger);
}

void send_page_for_mov_out_fs(int op_code, t_mov_out_fs *page, int client_socket, t_log *logger)
{
	t_package *package = create_empty_package(op_code);
	package->size = size_of_page_for_mov_out_fs(page->size);
	package->buffer = serialize_page_for_mov_out_fs(page);
	send_package(package, client_socket, logger);
}

void send_page_for_swap(int op_code, t_pag_swap *page, int client_socket, t_log *logger)
{
	t_package *package = create_empty_package(op_code);
	package->size = size_of_page_for_swap(page->page_info_size);
	package->buffer = serialize_page_for_swap(page);
	send_package(package, client_socket, logger);
	destroy_page_for_swap(page);
}

void send_page(int op_code, t_pag *page, int client_socket, t_log *logger)
{
	t_package *package = create_empty_package(op_code);
	package->size = size_of_page();
	package->buffer = serialize_page(page);
	send_package(package, client_socket, logger);
	// destroy_page(page);
}

t_pag *receive_page(int client_socket, t_log *logger)
{
	void *buffer = receive_buffer(client_socket, logger);
	return deserialize_page(buffer);
}

t_mov_out *receive_page_for_mov_out(int client_socket, t_log *logger)
{
	void *buffer = receive_buffer(client_socket, logger);
	return deserialize_page_for_mov_out(buffer);
}

t_mov_out_fs *receive_page_for_mov_out_fs(int client_socket, t_log *logger)
{
	void *buffer = receive_buffer(client_socket, logger);
	return deserialize_page_for_mov_out_fs(buffer);
}

t_pag_swap *receive_page_for_swap(int client_socket, t_log *logger)
{
	void *buffer = receive_buffer(client_socket, logger);
	return deserialize_page_for_swap(buffer);
}