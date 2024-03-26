#ifndef SRC_USER_MEMORY_PAGING_ALGORITHMS_H
#define SRC_USER_MEMORY_PAGING_ALGORITHMS_H

#include <user_memory/paging/pages.h>
#include <initial_configuration/memory_config.h>
//#include <user_memory/page_entry.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <semaphore.h>

pthread_mutex_t mtx_select_page;
extern int working;

void sort_pages_by_fifo(void);
void sort_pages_by_lru(void);
void init_sorter_thread(void);

#endif /* SRC_USER_MEMORY_PAGING_ALGORITHMS_H */