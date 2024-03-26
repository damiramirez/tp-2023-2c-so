#include <command/command.h>

// extern t_utils *utils;
sem_t wait;
char *message;
char *file_name;
int *block;
t_pag_swap *page_swap;
t_package *package;
t_pcb *pcb;
t_pcb *pcb_write;
t_fopen *open_data;
t_fchange *change_data;
t_fmodify *modify_data;
t_openf *seek_data;
t_openf *seek_data_write;

void *wait_for_commands(t_thread *thread_info)
{
	int op_code = receive_op_code(thread_info->port, thread_info->logger);
	if (op_code == -1)
	{
		if (strcmp(thread_info->dict_key, "KRL") == 0)
		{
			log_info(thread_info->logger, "Se corta por caida en Kernel");
			shutdown(thread_info->memory, SHUT_RDWR);
			close(thread_info->memory);
		}
		free(thread_info);
		sem_post(&wait);
		return -1;
	}
	while (op_code)
	{

		switch (op_code)
		{
		case ECHO_FILESYSTEM:
			message = receive_buffer(thread_info->port, thread_info->logger);
			log_info(thread_info->logger, "OpCode: %d and Message: %s", op_code, message);
			package = create_string_package(ECHO_MEMORY, "ECHO To Memory From FS");
			send_package(package, thread_info->port, thread_info->logger);
			free(message);
			break;

		case F_OPEN:
			pcb = receive_pcb(thread_info->port, thread_info->logger);
			open_data = (t_fopen *)pcb->params;
			int file_size = open_file(thread_info->logger, open_data->file_name);
			log_info(thread_info->logger, "Abrir Archivo: %s", open_data->file_name);
			package = create_integer_package(F_OPEN, file_size);
			send_package(package, thread_info->port, thread_info->logger);
			destroy_pcb(pcb);
			break;

		case F_CREATE:
			pcb = receive_pcb(thread_info->port, thread_info->logger);
			open_data = (t_fopen *)pcb->params;
			int ok = create_file(thread_info->logger, open_data->file_name);
			log_info(thread_info->logger, "Crear Archivo: %s", open_data->file_name);
			package = create_integer_package(F_CREATE, 0);
			send_package(package, thread_info->port, thread_info->logger);
			destroy_pcb(pcb);
			break;

		case F_TRUNCATE:
			pcb = receive_pcb(thread_info->port, thread_info->logger);
			change_data = (t_fchange *)pcb->params;
			truncate_file(thread_info->logger, change_data->file_name, change_data->value);
			log_info(thread_info->logger, "Truncar Archivo: %s - Tamaño: %d", change_data->file_name, change_data->value);
			package = create_integer_package(F_TRUNCATE, 0);
			send_package(package, thread_info->port, thread_info->logger);
			destroy_pcb(pcb);
			break;

		// SWAP
		// Recibo cantidad de bloques (int) - reservo - devuelvo lista?
		case GET_SWAP_BLOCKS:
			int *block_count = (int *)receive_buffer(thread_info->port, thread_info->logger);
			t_list *blocks_reserved = reserve_swap_blocks(*block_count);
			log_debug(thread_info->logger, "GET_SWAP_BLOCKS Memoria necesita %d de bloques SWAP reservados", *block_count);
			send_list(GET_SWAP_BLOCKS, blocks_reserved, thread_info->port, thread_info->logger);
			free(block_count);
			break;

		// Recibo bloque a leer (int) - leo data - devuelvo solo el void * con la informacion?
		case GET_FROM_SWAP:
			block = (int *)receive_buffer(thread_info->port, thread_info->logger);
			void *data = read_from_swap_block(*block);
			log_info(thread_info->logger, "Acceso a Bloque SWAP: “Acceso SWAP: %d”", *block);
			// Ver como enviar un void *
			package = create_void_package(GET_FROM_SWAP, fs_config->block_size, data);
			send_package(package, thread_info->port, thread_info->logger);
			free(block);
			break;

		// Recibo t_pag_swap - devuelvo un ok?
		case UPDATE_SWAP:
			page_swap = receive_page_for_swap(thread_info->port, thread_info->logger);
			write_to_swap_block(page_swap->swap_block, page_swap->page_content);
			log_info(thread_info->logger, "Actualizando Bloque SWAP: “Acceso SWAP: %d”", page_swap->swap_block);
			package = create_integer_package(UPDATE_SWAP, 0);
			send_package(package, thread_info->port, thread_info->logger);
			destroy_page_for_swap(page_swap);
			break;

		// Recibo una lista o que de bloques a liberar - libero - devuelvo ok?
		case FREE_PAGES:
			t_list *free_blocks = receive_list(thread_info->port, thread_info->logger);
			free_swap_blocks(free_blocks);
			log_info(thread_info->logger, "Libero %d bloques de SWAP", list_size(free_blocks));
			list_destroy(free_blocks);
			package = create_integer_package(FREE_PAGES, 0);
			send_package(package, thread_info->port, thread_info->logger);
			break;

		case F_READ:
			pcb = receive_pcb(thread_info->port, thread_info->logger);
			modify_data = (t_fmodify *)pcb->params;
			bool _find_filename(t_openf * openf)
			{
				return strcmp(openf->file, modify_data->file_name) == 0;
			};
			seek_data = list_find(pcb->open_files, _find_filename);
			int memory_address1 = (modify_data->page->page_number * fs_config->block_size) + modify_data->page->displacement;
			void *data2 = read_file(seek_data->file, seek_data->seek);
			log_info(thread_info->logger, "Leer Archivo: %s - Puntero: %d - Memoria: %d", seek_data->file, floor(seek_data->seek / fs_config->block_size), memory_address1);
			t_mov_out_fs *mov_out = page_for_mov_out_fs(pcb->pid, modify_data->page->page_number, modify_data->page->displacement, data2, fs_config->block_size);
			send_page_for_mov_out_fs(MOV_OUT_FS, mov_out, thread_info->memory, thread_info->logger);
			destroy_pcb(pcb);
			destroy_page_for_mov_out_fs(mov_out);
			break;

		case MOV_OUT_FS:
			int *ok2 = (int *)receive_buffer(thread_info->port, thread_info->logger);
			package = create_integer_package(F_READ, 0);
			send_package(package, thread_info->socket, thread_info->logger);
			free(ok2);
			break;

		case F_WRITE:
			pcb_write = receive_pcb(thread_info->port, thread_info->logger);
			modify_data = (t_fmodify *)pcb_write->params;
			bool _find_filename2(t_openf * openf)
			{
				return strcmp(openf->file, modify_data->file_name) == 0;
			};
			seek_data_write = list_find(pcb_write->open_files, _find_filename2);
			send_page(MOV_IN_FS, modify_data->page, thread_info->memory, thread_info->logger);
			break;

		case MOV_IN_FS:
			void *data_mov_in = receive_buffer(thread_info->port, thread_info->logger);
			int memory_address = (modify_data->page->page_number * fs_config->block_size) + modify_data->page->displacement;
			write_file(seek_data_write->file, seek_data_write->seek, data_mov_in);
			log_info(thread_info->logger, "Escribir Archivo: %s - Puntero: %d - Memoria: %d", seek_data_write->file, floor(seek_data_write->seek / fs_config->block_size), memory_address);
			package = create_integer_package(F_WRITE, 0);
			send_package(package, thread_info->socket, thread_info->logger);
			free(data_mov_in);
			destroy_pcb(pcb_write);
			break;

		default:
			if (strcmp(thread_info->dict_key, "KRL") == 0)
			{
				log_info(thread_info->logger, "Se corta por caida en Kernel");
				shutdown(thread_info->memory, SHUT_RDWR);
				close(thread_info->memory);
			}
			log_error(thread_info->logger, "Unknown OpCode %d - key %s", op_code, thread_info->dict_key);
			free(thread_info);
			sem_post(&wait);
			return NULL;
		}
		op_code = receive_op_code(thread_info->port, thread_info->logger);
		if (op_code == -1)
		{
			if (strcmp(thread_info->dict_key, "KRL") == 0)
			{
				log_info(thread_info->logger, "Se corta por caida en Kernel");
				shutdown(thread_info->memory, SHUT_RDWR);
				close(thread_info->memory);
			}
			free(thread_info);
			sem_post(&wait);
			return NULL;
		};
	}
	if (strcmp(thread_info->dict_key, "KRL") == 0)
	{
		log_info(thread_info->logger, "Se corta por caida en Kernel");
		shutdown(thread_info->memory, SHUT_RDWR);
		close(thread_info->memory);
	}
	free(thread_info);
	sem_post(&wait);
	return NULL;
}

// Return file size si existe
// Return informar que el archivo no existe
int open_file(t_log *logger, char *file_name)
{
	log_debug(logger, "Abrir Archivo: %s", file_name);
	t_fcb *file = find_fcb_file(file_name);
	if (file)
	{
		log_debug(logger, "Archivo %s abierto", file_name);
		return file->file_size;
	}

	log_debug(logger, "Archivo %s no existe", file_name);
	return -1;
}

// Return OK
int create_file(t_log *logger, char *file_name)
{
	t_fcb *file = find_fcb_file(file_name);

	if (!file)
	{
		create_fcb_file(file_name);
	}

	return 1;
}

void truncate_file(t_log *logger, char *file_name, int new_size)
{
	t_fcb *fcb = find_fcb_file(file_name);

	if (fcb->file_size == new_size)
	{
		log_debug(logger, "El nuevo tamaño es el mismo que antes, no se trunco.");
		return;
	}

	// Caso que el file tiene tamanio 0 y asigno el initial block y el tamanio
	if (fcb->file_size == 0)
	{
		int first_block = assign_block_size(new_size);
		fcb->initial_block = first_block;
	}
	// Tamanio nuevo mayor que al del archivo
	else if (fcb->file_size < new_size)
	{
		int current_size = (fcb->file_size / fs_config->block_size) + 1;
		int new_block_count = (new_size / fs_config->block_size) + 1;

		log_debug(logger, "Cantidad de bloques actuales: %d - Cantidad de bloques total nuevos %d", current_size, new_block_count);

		int blocks_to_add = new_block_count - current_size - 1;

		log_debug(logger, "Cantidad de bloques que hay que agregar: %d", blocks_to_add);

		add_blocks(fcb->initial_block, blocks_to_add);
	}
	else
	{
		int current_size = (fcb->file_size / fs_config->block_size) + 1;
		int blocks_needed = (new_size / fs_config->block_size) + 1;

		log_debug(logger, "Cantidad de bloques actuales: %d - Cantidad de bloques nuevos %d", current_size, blocks_needed);

		free_blocks(fcb->initial_block, blocks_needed);
	}

	fcb->file_size = new_size;

	// Actualizar file FCB
	update_fcb(fcb);
}

// TODO: REVISAR
void wait_in_every_port(int memory, int kernel, t_log *logger)
{
	sem_init(&wait, 0, 0);
	for (int i = 0; i < 2; i++)
	{
		pthread_t thread_id;
		t_thread *thread_info = malloc(sizeof(t_thread));
		switch (i)
		{
		case 0:
			thread_info->dict_key = "MRY";
			log_info(logger, "Iniciada thread de Memory");
			thread_info->port = memory;
			thread_info->socket = kernel;
			break;
		case 1:
			thread_info->dict_key = "KRL";
			log_info(logger, "Iniciada thread de Kernel");
			thread_info->port = kernel;
			thread_info->memory = memory;
			break;
		default:
			break;
		}
		thread_info->logger = logger;
		pthread_create(&thread_id, NULL, (void *)wait_for_commands, thread_info);
		pthread_detach(thread_id);
	}
	for (int i = 0; i < 2; i++)
	{
		sem_wait(&wait);
	}
}