#include "file_system.h"

t_lock *create_lock(t_pcb *pcb, bool is_write_lock)
{
    t_lock *lock = malloc(sizeof(*lock));
    lock->participants = list_create();
    list_add(lock->participants, pcb->pid);
    lock->is_write_lock = is_write_lock;
    sem_init(&lock->write_locked, 0, 0);
    return lock;
}

t_lock* add_read_lock(t_open_file *file, t_pcb *pcb, bool is_blocked)
{
    bool _is_read_lock(t_lock * lock)
    {
        return lock->is_write_lock == false;
    };
    pthread_mutex_lock(&open_files_global_table_mutex);
    t_lock *read_lock = list_find(file->locks, _is_read_lock);
    if (read_lock == NULL)
    {
        read_lock = create_lock(pcb, false);
        list_add(file->locks, read_lock);
        pthread_mutex_unlock(&open_files_global_table_mutex);
    }
    else
    {
        list_add(read_lock->participants, pcb->pid);
        pthread_mutex_unlock(&open_files_global_table_mutex);
    }
    read_lock->is_blocked = is_blocked;
    return read_lock;
}

t_openf *find_seek(t_pcb *pcb, char *file_name)
{
    bool _file_by_name_in_list(t_openf * openf)
    {
        return strcmp(openf->file, file_name) == 0;
    };
    return list_find(pcb->open_files, _file_by_name_in_list);
}

void f_seek(t_pcb *pcb, t_log *logger)
{
    t_fchange *fseek_params = (t_fchange *)pcb->params;
    t_openf *openf = find_seek(pcb, fseek_params->file_name);
    openf->seek = fseek_params->value;
    log_info(logger, "PID: %d - Actualizar puntero Archivo: %s - Puntero %d", pcb->pid, fseek_params->file_name, fseek_params->value);
    pthread_mutex_lock(&mtx_execute_process);
	destroy_pcb(estado_EXEC);
    estado_EXEC = pcb;
    pthread_mutex_unlock(&mtx_execute_process);
    sem_post(&executing_process);
}

void *treat_wait_for_read_lock(t_wait_for_read_lock *interrupted_info)
{
    t_lock* lock = add_read_lock(interrupted_info->open_file, interrupted_info->pcb, true);
    sem_wait(&lock->write_locked);
    log_debug(interrupted_info->logger, "Se desbloqueo %d dsps de un close", interrupted_info->pcb->pid);
    log_info(interrupted_info->logger, "PID: %d - Estado Anterior: %d - Estado Actual: %d", interrupted_info->pcb->pid, BLOCKED, READY);
    agregar_pcb_a_cola_READY(interrupted_info->pcb, interrupted_info->logger);
    free(interrupted_info);
}

void *treat_wait_for_write_lock(t_wait_for_write_lock *interrupted_info)
{
    t_lock *lock = create_lock(interrupted_info->pcb, true);
    lock->is_blocked=true;
    pthread_mutex_lock(&open_files_global_table_mutex);
    list_add(interrupted_info->open_file->locks, lock);
    pthread_mutex_unlock(&open_files_global_table_mutex);
    sem_wait(&lock->write_locked);
    log_info(interrupted_info->logger, "PID: %d - Estado Anterior: %d - Estado Actual: %d", interrupted_info->pcb->pid, BLOCKED, READY);
    agregar_pcb_a_cola_READY(interrupted_info->pcb, interrupted_info->logger);
    free(interrupted_info);
}

void f_open(t_pcb *pcb, int fs_socket, t_log *logger)
{
    t_fopen *open_data = pcb->params;
    t_openf *seek_file = malloc(sizeof(*seek_file));
    seek_file->seek = 0;
    seek_file->file = string_duplicate(open_data->file_name);
    list_add(pcb->open_files, seek_file);
    t_open_file *file;
    pthread_mutex_lock(&open_files_global_table_mutex);
    if (dictionary_has_key(open_files_global_table, open_data->file_name))
    {
        file = dictionary_get(open_files_global_table, open_data->file_name);
        pthread_mutex_unlock(&open_files_global_table_mutex);
        send_pcb(F_OPEN_COMMAND, pcb, fs_socket, logger);
        int op_code = receive_op_code(fs_socket, logger);
        if (op_code != F_OPEN_COMMAND)
        {
            log_warning("Invalid OP_CODE in F_OPEN: %d", op_code);
        }
        int *file_size = (int *)receive_buffer(fs_socket, logger);
        if (*file_size == -1)
        {
            log_warning("Invalid response in F_OPEN: %d", *file_size);
        }
        free(file_size);
    }
    else
    {
        file = malloc(sizeof(*file));
        file->locks = list_create();
        file->size = 0;
        dictionary_put(open_files_global_table, open_data->file_name, file);
        pthread_mutex_unlock(&open_files_global_table_mutex);
        send_pcb(F_CREATE_COMMAND, pcb, fs_socket, logger);
        int op_code = receive_op_code(fs_socket, logger);
        if (op_code != F_CREATE_COMMAND)
        {
            log_warning("Invalid OP_CODE in F_CREATE: %d", op_code);
        }
        int *ok = (int *)receive_buffer(fs_socket, logger);
        if (*ok != 0)
        {
            log_warning("Invalid response in F_CREATE: %d", *ok);
        }
        free(ok);
    }
    log_info(logger, "PID: %d - Abrir Archivo: %s", pcb->pid, open_data->file_name);

    if (strcmp(open_data->open_mode, "R") == 0)
    {
        if (file->locks->elements_count != 0 && ((t_lock *)list_get(file->locks, 0))->is_write_lock == true)
        {
            destroy_executing_process();
            pcb->estado = BLOCKED;
            log_info(logger, "PID: %d - Estado Anterior: %d - Estado Actual: %d", pcb->pid, EXEC, BLOCKED);
            pthread_t wait_process_for_start_lock;
            t_wait_for_read_lock *interrupted_info = malloc(sizeof(*interrupted_info));
            interrupted_info->pcb = pcb;
            interrupted_info->logger = logger;
            interrupted_info->open_file = file;
            pthread_create(&wait_process_for_start_lock, NULL, (void *)treat_wait_for_read_lock, interrupted_info);
            pthread_detach(wait_process_for_start_lock);
            sem_post(&proceso_en_cola_ready);
        }
        else
        {
            add_read_lock(file, pcb, false);
            pthread_mutex_lock(&mtx_execute_process);
            destroy_pcb(estado_EXEC);
            estado_EXEC = pcb;
            pthread_mutex_unlock(&mtx_execute_process);
            sem_post(&executing_process);
        }
    }
    else if (strcmp(open_data->open_mode, "W") == 0)
    {
        if (file->locks->elements_count != 0)
        {
            destroy_executing_process();
            pcb->estado = BLOCKED;
            log_info(logger, "PID: %d - Estado Anterior: %d - Estado Actual: %d", pcb->pid, EXEC, BLOCKED);
            pthread_t wait_process_for_start_lock;
            t_wait_for_write_lock *interrupted_info = malloc(sizeof(*interrupted_info));
            interrupted_info->pcb = pcb;
            interrupted_info->logger = logger;
            interrupted_info->open_file = file;
            pthread_create(&wait_process_for_start_lock, NULL, (void *)treat_wait_for_write_lock, interrupted_info);
            pthread_detach(wait_process_for_start_lock);
            sem_post(&proceso_en_cola_ready);
        }
        else
        {
            t_lock *lock = create_lock(pcb, true);
            lock->is_blocked = false;
            pthread_mutex_lock(&open_files_global_table_mutex);
            list_add(file->locks, lock);
            pthread_mutex_unlock(&open_files_global_table_mutex);
            pthread_mutex_lock(&mtx_execute_process);
            destroy_pcb(estado_EXEC);
            estado_EXEC = pcb;
            pthread_mutex_unlock(&mtx_execute_process);
            sem_post(&executing_process);
        }
    }
    else
    {
        log_error("Llego cualquier open_mode %s", open_data->open_mode);
    }
}

void f_close(t_pcb *pcb, int fs_socket, t_log *logger)
{
    char *file_name = pcb->params;
    pthread_mutex_lock(&open_files_global_table_mutex);
    t_open_file *file = dictionary_get(open_files_global_table, file_name);
    t_lock *lock = list_get(file->locks, 0);
    pthread_mutex_unlock(&open_files_global_table_mutex);
    log_info(logger, "PID: %d - Cerrar Archivo: %s", pcb->pid, file_name);
    bool _find_openf(t_openf * openf)
    {
        return strcmp(openf->file, file_name) == 0;
    };
    t_openf *openf = list_remove_by_condition(pcb->open_files, _find_openf);
    free(openf->file);
    free(openf);
    close_lock(pcb, file, lock, logger);
    pthread_mutex_lock(&mtx_execute_process);
	destroy_pcb(estado_EXEC);
    estado_EXEC = pcb;
    pthread_mutex_unlock(&mtx_execute_process);
    sem_post(&executing_process);
}

void f_truncate(t_pcb *pcb, int fs_socket, t_log *logger)
{
    destroy_executing_process();
    t_fchange *truncate_data = pcb->params;
    pthread_mutex_lock(&open_files_global_table_mutex);
    t_open_file *file = dictionary_get(open_files_global_table, truncate_data->file_name);
    file->size = truncate_data->value;
    pthread_mutex_unlock(&open_files_global_table_mutex);
    pcb->estado = BLOCKED;
    log_info(logger, "PID: %d - Estado Anterior: %d - Estado Actual: %d", pcb->pid, EXEC, BLOCKED);
    pthread_t call_fs_thread;
    t_fs_call *interrupted_info = malloc(sizeof(*interrupted_info));
    interrupted_info->pcb = pcb;
    interrupted_info->logger = logger;
    interrupted_info->fs_socket = fs_socket;
    interrupted_info->command = F_TRUNCATE_COMMAND;
    interrupted_info->open_file = file;
    pthread_create(&call_fs_thread, NULL, (void *)treat_fs_function, interrupted_info);
    pthread_detach(call_fs_thread);
    sem_post(&proceso_en_cola_ready);
}

void *treat_fs_function(t_fs_call *data)
{
    send_pcb(data->command, data->pcb, data->fs_socket, data->logger);
    int op_code = receive_op_code(data->fs_socket, data->logger);
    if (op_code != data->command)
    {
        log_warning("Invalid OP_CODE in %d: %d", data->command, op_code);
    }
    int *ok = (int *)receive_buffer(data->fs_socket, data->logger);
    if (*ok != 0)
    {
        log_warning("Invalid response in %d: %d", data->command, *ok);
    }
    free(ok);
    switch (data->command)
    {
    case F_TRUNCATE_COMMAND:
        t_fchange *truncate_data = data->pcb->params;
        log_info(data->logger, "PID: %d - Archivo: %s - Tamaño: %d", data->pcb->pid, truncate_data->file_name, truncate_data->value);
        break;
    case F_READ_COMMAND:
        t_fmodify *read_data = data->pcb->params;
        t_openf *read_seek = find_seek(data->pcb, read_data->file_name);
        log_info(data->logger, "PID: %d - Leer Archivo: %s - Puntero %d - Dirección Memoria [%d,%d] - Tamaño %d", data->pcb->pid, read_data->file_name, read_seek->seek, read_data->page->page_number, read_data->page->displacement, data->open_file->size);
        break;
    case F_WRITE_COMMAND:
        t_fmodify *write_data = data->pcb->params;
        t_openf *write_seek = find_seek(data->pcb, write_data->file_name);
        log_info(data->logger, "PID: %d - Escribir Archivo: %s - Puntero %d - Dirección Memoria [%d,%d] - Tamaño %d", data->pcb->pid, write_data->file_name, write_seek->seek, write_data->page->page_number, write_data->page->displacement, data->open_file->size);
        break;
    default:
        log_warning(data->logger, "Invalid opCode: %d", data->command);
        break;
    }
    log_info(data->logger, "PID: %d - Estado Anterior: %d - Estado Actual: %d", data->pcb->pid, BLOCKED, READY);
    agregar_pcb_a_cola_READY(data->pcb, data->logger);
    free(data);
}

void f_read(t_pcb *pcb, int fs_socket, t_log *logger)
{
    destroy_executing_process();
    pcb->estado = BLOCKED;
    log_info(logger, "PID: %d - Estado Anterior: %d - Estado Actual: %d", pcb->pid, EXEC, BLOCKED);
    pthread_t call_fs_thread;
    t_fmodify *modify_data = (t_fmodify *)pcb->params;
    pthread_mutex_lock(&open_files_global_table_mutex);
    t_open_file *file = dictionary_get(open_files_global_table, modify_data->file_name);
    pthread_mutex_unlock(&open_files_global_table_mutex);
    t_fs_call *interrupted_info = malloc(sizeof(*interrupted_info));
    interrupted_info->pcb = pcb;
    interrupted_info->logger = logger;
    interrupted_info->fs_socket = fs_socket;
    interrupted_info->command = F_READ_COMMAND;
    interrupted_info->open_file = file;
    pthread_create(&call_fs_thread, NULL, (void *)treat_fs_function, interrupted_info);
    pthread_detach(call_fs_thread);
    sem_post(&proceso_en_cola_ready);
}

void f_write(t_pcb *pcb, int fs_socket, t_log *logger)
{
    destroy_executing_process();
    t_fmodify *modify_data = (t_fmodify *)pcb->params;
    pthread_mutex_lock(&open_files_global_table_mutex);
    t_open_file *file = dictionary_get(open_files_global_table, modify_data->file_name);
    pthread_mutex_unlock(&open_files_global_table_mutex);
    t_lock *lock = list_get(file->locks, 0);
    // bool _same_pid(uint32_t pid)
    // {
    //     return pcb->pid == pid;
    // };
    // bool is_read_lock = list_any_satisfy(read_lock->participants, _same_pid);
    if (!lock->is_write_lock)
    {
        send_to_exit(pcb, logger, INVALID_WRITE);
    }
    else
    {
        pcb->estado = BLOCKED;
        log_info(logger, "PID: %d - Estado Anterior: %d - Estado Actual: %d", pcb->pid, EXEC, BLOCKED);
        pthread_t call_fs_thread;
        t_fs_call *interrupted_info = malloc(sizeof(*interrupted_info));
        interrupted_info->pcb = pcb;
        interrupted_info->logger = logger;
        interrupted_info->fs_socket = fs_socket;
        interrupted_info->command = F_WRITE_COMMAND;
        interrupted_info->open_file = file;
        pthread_create(&call_fs_thread, NULL, (void *)treat_fs_function, interrupted_info);
        pthread_detach(call_fs_thread);
        sem_post(&proceso_en_cola_ready);
    }
}