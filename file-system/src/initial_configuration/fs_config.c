#include <initial_configuration/fs_config.h>

t_fs_config *fs_config;

void init_fs_config(t_config *config, t_fs_config *fs_config)
{
    fs_config->path_fat = config_get_string_value(config, "PATH_FAT");
    fs_config->path_block = config_get_string_value(config, "PATH_BLOQUES");
    fs_config->path_fcb = config_get_string_value(config, "PATH_FCB");
    fs_config->block_total_count = config_get_int_value(config, "CANT_BLOQUES_TOTAL");
    fs_config->block_swap_count = config_get_int_value(config, "CANT_BLOQUES_SWAP");
    fs_config->block_size = config_get_int_value(config, "TAM_BLOQUE");
    fs_config->block_time_delay = config_get_int_value(config, "RETARDO_ACCESO_BLOQUE");
    fs_config->fat_time_delay = config_get_int_value(config, "RETARDO_ACCESO_FAT");
}

void destroy_fs_config(t_fs_config *fs_config)
{
    free(fs_config);
}