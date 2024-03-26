#ifndef FS_CONFIG_H_
#define FS_CONFIG_H_

#include <config/config.h>
#include <commons/config.h>

typedef struct
{
    char *path_fat;
    char *path_block;
    char *path_fcb;
    int block_total_count;
    int block_swap_count;
    int block_size;
    int block_time_delay;
    int fat_time_delay;
} t_fs_config;

typedef struct
{
    char *file_name;
    int file_size;
    int initial_block;
} t_fcb;

void init_fs_config(t_config *config, t_fs_config *fs_config);
void destroy_fs_config(t_fs_config *fs_config);

extern t_fs_config *fs_config;

#endif /* FS_CONFIG_H_ */