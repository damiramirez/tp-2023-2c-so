#ifndef SRC_USER_MEMORY_REAL_MEMORY_H
#define SRC_USER_MEMORY_REAL_MEMORY_H

#include <stdlib.h>
#include <initial_configuration/memory_config.h>
#include <user_memory/paging/algorithms.h>
#include <user_memory/paging/pages.h>
#include <user_memory/swaping/swaping.h>
//#include <user_memory/page_entry.h>
#include <command/command.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <pthread.h>

typedef struct
{
    void *frames;
    t_bitarray *frame_table;
} t_user_space;

typedef struct
{
    bool available;
    int frame_number;
} t_frame_search;

pthread_mutex_t mtx_frame_access;
extern t_user_space real_memory;

void init_real_memory(void);
void free_memory(t_log *logger);
void free_frame(int frame);
t_frame_search check_available_frames(void);
t_page_entry *reference_page(uint32_t pid, int page_number, t_log *logger);
void *read_frame(int real_address, size_t size);
void write_on_frame(int real_address, size_t size, void *data);
void load_page(uint32_t pid, int page_number, int fs_socket, void *page_data, t_log *logger);

#endif /* SRC_USER_MEMORY_REAL_MEMORY_H */