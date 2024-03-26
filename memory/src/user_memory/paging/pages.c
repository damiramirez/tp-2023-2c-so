#include <user_memory/paging/pages.h>

pthread_mutex_t page_reference;
sem_t sort_pages;

pthread_mutex_t table;

t_page_entry *last_page_referenced;
t_list *pages_to_replace;
t_list *page_tables;

void send_page_size_to_cpu(t_conn *conn, t_utils *utils)
{
	receive_op_code(conn->socket_cpu, utils->logger);
	t_package *package_page = create_integer_package(PAGE_SIZE, memory_config.page_size);
	send_package(package_page, conn->socket_cpu, utils->logger);
}

t_page_entry *page_create(uint32_t pid, int bit_modified, int bit_precense, int frame, int page_number, int swap_block)
{
	t_page_entry *page = malloc(sizeof(*page));

	page->pid = pid;
	page->bit_modified = bit_modified;
	page->bit_precense = bit_precense;
	page->frame_number = frame;
	page->page_number = page_number;
	page->swap_position = swap_block;

	return page;
}

void page_table_create(t_pcb *pcb, t_list *swap_blocks, t_log *logger)
{
	int total_pages = floor(pcb->tamanio / memory_config.page_size);
	t_page_table *page_table = malloc(sizeof(*page_table));

	page_table->process_pid = pcb->pid;
	page_table->pages = list_create();

	for (int i = 0; i < total_pages; i++)
	{
		int *swap_block_ptr = (int *)list_get(swap_blocks, i);
		list_add(page_table->pages, page_create(pcb->pid, 0, 0, 0, i, *swap_block_ptr));
		free(swap_block_ptr);
	}
	list_destroy(swap_blocks);
	list_add(page_tables, page_table);

	log_info(logger, "PID: %d - Tamaño: %d", pcb->pid, total_pages);
}

t_page_table *search_page_table(uint32_t pid)
{
	int _is_table(t_page_table * page_table)
	{
		return page_table->process_pid == pid;
	};

	pthread_mutex_lock(&table);
	t_page_table *page_table = list_find(page_tables, (void *)_is_table);
	pthread_mutex_unlock(&table);

	return page_table;
}

t_page_entry *get_page(uint32_t pid, int page_number)
{
	t_page_table *page_table = search_page_table(pid);

	return list_get(page_table->pages, page_number);
}

void send_page_fault(int socket, t_log *logger)
{
	t_package *package = create_integer_package(PAGE_FAULT_COMMAND, 0);
	send_package(package, socket, logger);
}

void send_page_frame(t_page_entry *page, int socket, t_log *logger)
{
	t_package *package = create_integer_package(PAGE_FRAME, page->frame_number);
	send_package(package, socket, logger);
}

void destroy_page_entry(t_page_entry *page)
{
	if (page->bit_precense == 1)
	{
		free_frame(page->frame_number);
	}
	list_remove_element(pages_to_replace, page);
	free(page);
}

void destroy_page_table(t_page_table *page_table, t_log *logger)
{
	log_info(logger, "PID: %d - Tamaño: %d", page_table->process_pid, list_size(page_table->pages));
	pthread_mutex_lock(&table);
	list_destroy_and_destroy_elements(page_table->pages, (void *)destroy_page_entry);
	pthread_mutex_unlock(&table);
	free(page_table);
}

void destroy_page_tables(t_log *logger)
{
	for (int i = 0; i < list_size(page_tables); i++)
	{
		destroy_page_table(list_get(page_tables, i), logger);
	}
	list_destroy(page_tables);
}
