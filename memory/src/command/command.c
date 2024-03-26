#include <command/command.h>

t_pcb *pcb_create;
t_pcb *pcb_end;
t_pcb *pcb_fetch;
t_pag *received_page;
t_page_entry *page;
t_pag *received_page_mov_in;
t_pag *received_page_load_page;
t_page_entry *page_mov_in;
t_list *list_page;
sem_t wait;
int size;

pthread_mutex_t *mutex_load_page;

void *wait_for_command(t_thread *thread_info)
{
    list_page = list_create();
    int op_code = receive_op_code(thread_info->port, thread_info->logger);
    if (op_code == -1)
    {
        free(thread_info);
        sem_post(&wait);
        return NULL;
    };
    while (op_code)
    {
        switch (op_code)
        {
        case ECHO_MEMORY:
            char *message = receive_buffer(thread_info->port, thread_info->logger);
            log_info(thread_info->logger, "OpCode: %d and Message: %s", op_code, message);
            free(message);
            break;
        case CREATE_PROCESS:
            pcb_create = receive_pcb(thread_info->port, thread_info->logger);
            log_debug(thread_info->logger, "PIDO SWAP");
            get_swap_blocks(pcb_create->tamanio, thread_info->conn->socket_filesystem, thread_info->logger, thread_info->dict_key);
            break;
        case GET_SWAP_BLOCKS:
            t_list *swap_blocks = receive_list(thread_info->conn->socket_filesystem, thread_info->logger);
            log_debug(thread_info->logger, "Cantidad de bloques %d", list_size(swap_blocks));
            int is_ok = create_process(thread_info->logger, pcb_create, swap_blocks);
            t_package *package_process = create_integer_package(PROCESS_OK, is_ok);
            send_package(package_process, thread_info->conn->socket_kernel, thread_info->logger);
            destroy_pcb(pcb_create);
            break;
        case LOAD_PAGE:
            received_page_load_page = receive_page(thread_info->port, thread_info->logger);

            pthread_mutex_lock(&mutex_load_page);
            list_add(list_page, received_page_load_page);
            pthread_mutex_unlock(&mutex_load_page);
            log_debug(thread_info->logger, "Se hace un page fault %d", received_page_load_page->page_number);
            page = reference_page(received_page_load_page->pid, received_page_load_page->page_number, thread_info->logger);
            get_page_info(page->swap_position, thread_info->conn->socket_filesystem, thread_info->logger);
            break;
        case GET_FROM_SWAP:
            void *page_data = receive_buffer(thread_info->port, thread_info->logger);
            t_pag *page_swap;
            pthread_mutex_lock(&mutex_load_page);
            if (list_page->elements_count != 0)
            {
                page_swap = list_remove(list_page, 0);
                pthread_mutex_unlock(&mutex_load_page);
                load_page(page_swap->pid, page_swap->page_number, thread_info->conn->socket_filesystem, page_data, thread_info->logger);
                t_package *result_package = create_integer_package(LOAD_PAGE, 0);
                send_package(result_package, thread_info->conn->socket_kernel, thread_info->logger);
                destroy_page(page_swap);
            }
            else
            {
                log_warning(thread_info->logger, "No hay page en lista");
                pthread_mutex_unlock(&mutex_load_page);
                t_package *result_package = create_integer_package(LOAD_PAGE, 0);
                send_package(result_package, thread_info->conn->socket_kernel, thread_info->logger);
            }
            break;
        case FREE_PAGES:
            int *ok = receive_buffer(thread_info->port, thread_info->logger);
            if (*ok != 0)
            {
                log_warning(thread_info->logger, "Error mientras se liberaban paginas, %d", *ok);
            }
            free(ok);
            break;
        case PAGE_NUMBER:
            received_page = receive_page(thread_info->port, thread_info->logger);
            page = get_page(received_page->pid, received_page->page_number);

            (page == NULL || !page->bit_precense)
                ? send_page_fault(thread_info->port, thread_info->logger)
                : send_page_frame(page, thread_info->port, thread_info->logger);
            destroy_page(received_page);
            break;
        case FETCH_INSTRUCTION:
            pcb_fetch = receive_pcb(thread_info->port, thread_info->logger);
            char *next_instruction = fetch_next_instruction(pcb_fetch->pid, pcb_fetch->programCounter, thread_info->logger);
            log_debug(thread_info->logger, "Next instruction: %s", next_instruction);
            t_package *package_instruct = create_string_package(FETCH_INSTRUCTION, next_instruction);
            usleep(memory_config.time_delay * 1000);
            send_package(package_instruct, thread_info->port, thread_info->logger);
            destroy_pcb(pcb_fetch);
            break;
        case MOV_OUT:
            t_mov_out *mov_out_page = receive_page_for_mov_out(thread_info->port, thread_info->logger);
            page = reference_page(mov_out_page->pid, mov_out_page->page_number, thread_info->logger);
            int address1 = page->frame_number * memory_config.page_size + mov_out_page->displacement;
            page->bit_modified = 1;
            write_on_frame(address1, sizeof(uint32_t), &mov_out_page->register_value);
            log_info(thread_info->logger, "PID: %d - Accion: ESCRIBIR - Direccion fisica: %d", page->pid, address1);
            t_package *result_package1 = create_integer_package(MOV_OUT, 0);
            send_package(result_package1, thread_info->port, thread_info->logger);
            destroy_page_for_mov_out(mov_out_page);
            break;
        case MOV_IN:
            received_page_mov_in = receive_page(thread_info->port, thread_info->logger);
            page = reference_page(received_page_mov_in->pid, received_page_mov_in->page_number, thread_info->logger);
            int address2 = page->frame_number * memory_config.page_size + received_page_mov_in->displacement;
            uint32_t *value_in_frame = (uint32_t *)read_frame(address2, sizeof(uint32_t));
            log_info(thread_info->logger, "PID: %d - Accion: LEER - Direccion fisica: %d", page->pid, address2);
            log_debug(thread_info->logger, "VALUE IN FRAME: %d", *value_in_frame);
            t_package *result_package2 = create_uint32_package(MOV_IN, *value_in_frame);
            send_package(result_package2, thread_info->port, thread_info->logger);
            free(value_in_frame);
            destroy_page(received_page_mov_in);
            break;
        case MOV_OUT_FS:
            t_mov_out_fs *mov_out_page_fs = receive_page_for_mov_out_fs(thread_info->port, thread_info->logger);
            page = reference_page(mov_out_page_fs->pid, mov_out_page_fs->page_number, thread_info->logger);
            int address3 = page->frame_number * memory_config.page_size;
            page->bit_modified = 1;
            write_on_frame(address3, mov_out_page_fs->size, mov_out_page_fs->register_value);
            log_info(thread_info->logger, "PID: %d - Accion: ESCRIBIR - Direccion fisica: %d", page->pid, address3);
            log_debug(thread_info->logger, "Escribi: %d", *(uint32_t *)mov_out_page_fs->register_value);
            t_package *result_package3 = create_integer_package(MOV_OUT_FS, 0);
            send_package(result_package3, thread_info->port, thread_info->logger);
            destroy_page_for_mov_out_fs(mov_out_page_fs);
            break;
        case MOV_IN_FS:
            received_page_mov_in = receive_page(thread_info->port, thread_info->logger);
            page_mov_in = reference_page(received_page_mov_in->pid, received_page_mov_in->page_number, thread_info->logger);
            int address4 = page_mov_in->frame_number * memory_config.page_size;
            void *value_in_frame2 = read_frame(address4, memory_config.page_size);
            log_info(thread_info->logger, "PID: %d - Accion: LEER - Direccion fisica: %d", page_mov_in->pid, address4);
            log_debug(thread_info->logger, "[FS] VALUE IN FRAME: %d", *(uint32_t *)value_in_frame2);
            t_package *result_package4 = create_void_package(MOV_IN_FS, memory_config.page_size, value_in_frame2);
            send_package(result_package4, thread_info->port, thread_info->logger);
            destroy_page(received_page_mov_in);
            break;
        case END_PROCESS:
            pcb_end = receive_pcb(thread_info->port, thread_info->logger);
            deallocate_process(
                pcb_end->pid,
                thread_info->conn->socket_filesystem,
                thread_info->logger);
            destroy_pcb(pcb_end);
            break;
        default:
            log_error(thread_info->logger, "Unknown OpCode %d - key %s", op_code, thread_info->dict_key);
            free(thread_info);
            sem_post(&wait);
            return NULL;
        }
        op_code = receive_op_code(thread_info->port, thread_info->logger);
        if (op_code == -1)
        {
            free(thread_info);
            sem_post(&wait);
            return NULL;
        };
    }
    free(thread_info->dict_key);
    free(thread_info);
    sem_post(&wait);
    return NULL;
}

void wait_in_every_port(t_conn *conn, t_log *logger)
{
    sem_init(&wait, 0, 0);
    for (int i = 0; i < MODULOS_A_CONECTAR; i++)
    {
        pthread_t thread_id;
        t_thread *thread_info = malloc(sizeof(t_thread));
        switch (i)
        {
        case 0:
            log_info(logger, "Iniciada thread de FileSystem");
            thread_info->port = conn->socket_filesystem;
            thread_info->dict_key = "FS";
            break;
        case 1:
            log_info(logger, "Iniciada thread de Kernel");
            thread_info->port = conn->socket_kernel;
            thread_info->dict_key = "KRL";
            break;
        case 2:
            log_info(logger, "Iniciada thread de CPU");
            thread_info->port = conn->socket_cpu;
            thread_info->dict_key = "CPU";
            break;
        default:
            break;
        }
        thread_info->logger = logger;
        thread_info->conn = conn;
        pthread_create(&thread_id, NULL, (void *)wait_for_command, thread_info);
        pthread_detach(thread_id);
    }
    for (int i = 0; i < MODULOS_A_CONECTAR; i++)
    {
        sem_wait(&wait);
    }
}