#include "fcb.h"

extern t_utils *utils;
extern t_fs_config *fs_config;
extern t_list *fcbs;

void create_fcb_file(char *file_name)
{
    char *full_path = string_from_format("%s/%s.fcb", fs_config->path_fcb, file_name);

    // Declaro struct de FCB para la lista
    t_fcb *fcb_struct = malloc(sizeof(t_fcb));
    fcb_struct->file_name = strdup(file_name);
    fcb_struct->file_size = 0;
    fcb_struct->initial_block = 0;

    int fd = open(full_path, O_CREAT | O_RDWR, S_IRWXU);

    if (fd != -1)
    {

        char *name = string_duplicate("NOMBRE_ARCHIVO=");
        string_append(&name, file_name);
        char *size = "\nTAMANIO_ARCHIVO=0";
        char *initial_block = "\nBLOQUE_INICIAL=0";

        char *content = string_new();
        string_append(&content, name);
        string_append(&content, size);
        string_append(&content, initial_block);

        size_t content_size = string_length(content);
        ftruncate(fd, content_size);

        char *mapped_data = mmap(NULL, content_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        memcpy(mapped_data, content, content_size);
        msync(mapped_data, content_size, MS_SYNC);
        munmap(mapped_data, content_size);

        list_add(fcbs, fcb_struct);

        close(fd);
        free(content);
        free(name);
    }
    else
    {
        log_error(utils->logger, "No se pudo crear el archivo %s", file_name);
        free(fcb_struct->file_name);
        free(fcb_struct);
    }

    free(full_path);
}

// TODO: Ver si cuando corro el programa y existen FCB, cargarlos a la lista
t_fcb *find_fcb_file(char *file_name)
{
    // t_fcb *fcb = malloc(sizeof(fcb));
    for (int i = 0; i < list_size(fcbs); i++)
    {
        t_fcb *fcb = list_get(fcbs, i);
        if (strcmp(fcb->file_name, file_name) == 0)
        {
            return fcb;
        }
    }
    // free(fcb);
    return NULL;
}

void update_fcb(t_fcb *fcb)
{

    char *full_path = string_from_format("%s/%s.fcb", fs_config->path_fcb, fcb->file_name);
    int fd = open(full_path, O_RDWR);

    if (fd != -1)
    {
        char *size_str = string_from_format("TAMANIO_ARCHIVO=%d", fcb->file_size);
        char *initial_block_str = string_from_format("BLOQUE_INICIAL=%d", fcb->initial_block);
        char *content = string_from_format("NOMBRE_ARCHIVO=%s\n%s\n%s", fcb->file_name, size_str, initial_block_str);
        size_t content_size = strlen(content);

        ftruncate(fd, content_size);

        char *mapped_data = mmap(NULL, content_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        memcpy(mapped_data, content, content_size);
        msync(mapped_data, content_size, MS_SYNC);
        munmap(mapped_data, content_size);
        close(fd);
        free(content);
        free(initial_block_str);
        free(size_str);
    }
    else
    {
        log_error(utils->logger, "No se pudo abrir el archivo asociado al FCB %s", fcb->file_name);
    }

    free(full_path);
}

void print_fcb_list()
{
    log_debug(utils->logger, "Lista de archivos:");
    // t_fcb *fcb = malloc(sizeof(fcb));
    for (int i = 0; i < list_size(fcbs); i++)
    {
        t_fcb *fcb = list_get(fcbs, i);
        log_debug(utils->logger, "FCB numero: %d\n\tNombre: %s\n\tTamanio: %d\n\tBloque inicial: %d", i, fcb->file_name, fcb->file_size, fcb->initial_block);
    }
    // free(fcb);
}

void load_FCBs_from_directory(char *directory_path)
{
    log_debug(utils->logger, "Path %s", directory_path);

    DIR *dir = opendir(directory_path);

    if (dir != NULL)
    {
        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL)
        {
            if (string_contains(entry->d_name, ".fcb"))
            {
                log_debug(utils->logger, "Archivo: %s", entry->d_name);
                t_fcb *exist_fcb = malloc(sizeof(t_fcb));
                char* path = string_from_format("%s/%s", directory_path, entry->d_name);
                t_config *fcb_config = create_config(path, utils->logger);
                free(path);
                exist_fcb->file_size = config_get_int_value(fcb_config, "TAMANIO_ARCHIVO");
                exist_fcb->initial_block = config_get_int_value(fcb_config, "BLOQUE_INICIAL");
                char *file_name = config_get_string_value(fcb_config, "NOMBRE_ARCHIVO");
                exist_fcb->file_name = string_duplicate(file_name);
                list_add(fcbs, exist_fcb);
                config_destroy(fcb_config);
            }
            // exist_fcb->file_size =
        }

        closedir(dir);
    }

    log_debug(utils->logger, "Se inicio la lista con %d", list_size(fcbs));
}