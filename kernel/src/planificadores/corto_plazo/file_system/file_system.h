#ifndef SRC_PLANIFICADORES_CORTO_PLAZO_FILE_SYSTEM_FILE_SYSTEM_H_
#define SRC_PLANIFICADORES_CORTO_PLAZO_FILE_SYSTEM_FILE_SYSTEM_H_

#include <planificadores/planificador.h>

typedef struct {
    t_pcb* pcb;
    t_log* logger;
    t_open_file* open_file;
} t_wait_for_read_lock;

typedef struct {
    t_pcb* pcb;
    t_log* logger;
    t_open_file* open_file;
} t_wait_for_write_lock;

typedef struct {
    t_pcb* pcb;
    t_log* logger;
    int fs_socket;
    int command;
    t_open_file* open_file;
} t_fs_call;

t_lock* create_lock(t_pcb* pcb, bool is_write_lock);
t_lock* add_read_lock(t_open_file* file, t_pcb* pcb, bool is_blocked);
void f_seek(t_pcb* pcb, t_log* logger);
void* treat_wait_for_read_lock(t_wait_for_read_lock* interrupted_info);
void* treat_wait_for_write_lock(t_wait_for_write_lock* interrupted_info);
void f_open(t_pcb* pcb, int fs_socket, t_log* logger);
void f_close(t_pcb* pcb, int fs_socket, t_log* logger);
void f_truncate(t_pcb* pcb, int fs_socket, t_log* logger);
void f_read(t_pcb* pcb, int fs_socket, t_log* logger);
void f_write(t_pcb* pcb, int fs_socket, t_log* logger);
void* treat_fs_function(t_fs_call* data);
t_openf* find_seek(t_pcb* pcb, char* file_name);

#endif /* SRC_PLANIFICADORES_CORTO_PLAZO_FILE_SYSTEM_FILE_SYSTEM_H_ */