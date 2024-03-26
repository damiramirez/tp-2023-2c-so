#ifndef FAT_BLOQUE_H_
#define FAT_BLOQUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <config/config.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/memory.h>
#include <initial_configuration/fs_config.h>
#include <math.h>

// INIT
void init_fs_struct();
void destroy_fs();

// FAT
void create_fat_file();
void initialize_fat_list();
int find_free_block(int);
int find_free_current_block();
int assign_block_size(int file_size);
void assign_block(int file_block, uint32_t free_block);
void set_end_of_file(int file_block);
void print_fat();
int find_last_block(int initial_block);
void add_blocks(int initial_block, int additional_blocks);
t_list *recorrer_blocks(int initial_block);
void free_blocks(int initial_block, int blocks_needed);

// Bloques
void create_block_file();
t_list *reserve_swap_blocks(int);
void free_swap_blocks(t_list *blocks_to_release);
int find_free_swap_block();
void write_to_swap_block(int block_index, void *data);
void *read_from_swap_block(int block_index);
// Leer
void *read_file(char *file_name, int seek);
void write_file(char *file_name, int seek, void *data);

#endif /* FAT_BLOQUE_H_ */