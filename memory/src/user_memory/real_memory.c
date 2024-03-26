#include <user_memory/real_memory.h>

pthread_mutex_t mtx_frame_access;
t_user_space real_memory;

static void init_frame_table(void *user_space)
{
    int total_frames = memory_config.memory_size / memory_config.page_size;
    char *frames = string_repeat('0', total_frames);

    t_bitarray *frame_table = bitarray_create_with_mode(frames, sizeof(frames), LSB_FIRST);

    real_memory.frames = user_space;
    real_memory.frame_table = frame_table;
}

t_page_entry *reference_page(uint32_t pid, int page_number, t_log *logger)
{
    t_page_entry *page = get_page(pid, page_number);

    if (page == NULL)
        return NULL;

    if (page->bit_precense == 1)
        log_info(logger, "PID: %d - Pagina: %d - Marco: %d", page->pid, page->page_number, page->frame_number);

    pthread_mutex_lock(&page_reference);
    last_page_referenced = page;
    pthread_mutex_unlock(&page_reference);

    sem_post(&sort_pages);
    usleep(memory_config.time_delay * 1000);

    return page;
}

static void load_page_in_free_space(t_page_entry *page_referenced, int free_frame, void *page_data, t_log *logger)
{
    swap_in(page_referenced, free_frame, page_data, logger);
    bitarray_set_bit(real_memory.frame_table, free_frame);
}

static void page_replace(t_page_entry *page_referenced, int fs_socket, void *page_data, t_log *logger)
{
    pthread_mutex_lock(&mtx_select_page);
    t_page_entry *victim = (t_page_entry *)list_remove(pages_to_replace, 0);
    pthread_mutex_unlock(&mtx_select_page);

    swap_out(victim, fs_socket, logger);
    swap_in(page_referenced, victim->frame_number, page_data, logger);

    log_info(logger,
             "REEMPLAZO - Marco: %d - Page Out: %d-%d - Page In: %d-%d",
             victim->frame_number,
             victim->pid,
             victim->page_number,
             page_referenced->pid,
             page_referenced->page_number);
}

t_frame_search check_available_frames(void)
{
    int i = 0;
    int frame_table_length = memory_config.memory_size / memory_config.page_size;

    while (bitarray_test_bit(real_memory.frame_table, i))
    {
        if (i == frame_table_length)
        {
            t_frame_search result = {.available = false, .frame_number = 0};
            return result;
        }

        i++;
    }

    bitarray_set_bit(real_memory.frame_table, i);

    t_frame_search result = {.available = true, .frame_number = i};

    return result;
}

void free_frame(int frame)
{
    int frames_l = memory_config.memory_size / memory_config.page_size;
    bitarray_clean_bit(real_memory.frame_table, frame);
}

void load_page(uint32_t pid, int page_number, int fs_socket, void *page_data, t_log *logger)
{
    t_frame_search result = check_available_frames();
    t_page_entry *page = (t_page_entry *)get_page(pid, page_number);

    result.available
        ? load_page_in_free_space(page, result.frame_number, page_data, logger)
        : page_replace(page, fs_socket, page_data, logger);

    free(page_data);
}

void *read_frame(int real_address, size_t size)
{
    void *data = malloc(size);
    pthread_mutex_lock(&mtx_frame_access);
    memcpy(data, real_memory.frames + real_address, size);
    pthread_mutex_unlock(&mtx_frame_access);
    usleep(memory_config.time_delay * 1000);

    return data;
}

void write_on_frame(int real_address, size_t size, void *data)
{
    pthread_mutex_lock(&mtx_frame_access);
    memcpy(real_memory.frames + real_address, data, size);
    pthread_mutex_unlock(&mtx_frame_access);
    usleep(memory_config.time_delay * 1000);
}

void init_real_memory(void)
{
    void *user_space = malloc(memory_config.memory_size);
    page_tables = list_create();

    init_frame_table(user_space);
}

void free_memory(t_log *logger)
{
    working = 0;
    sem_post(&sort_pages);
    // list_destroy_and_destroy_elements(pages_to_replace, (void *)destroy_page_entry);
    destroy_page_tables(logger);
    list_destroy_and_destroy_elements(active_processes, (void *)deallocate_process);
    free(real_memory.frames);
    free(real_memory.frame_table->bitarray);
    bitarray_destroy(real_memory.frame_table);
    list_destroy(pages_to_replace);
}
