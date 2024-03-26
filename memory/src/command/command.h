#ifndef SRC_COMMAND_COMMAND_H_
#define SRC_COMMAND_COMMAND_H_

#define MODULOS_A_CONECTAR 3

#include <pthread.h>
#include <unistd.h>
#include <commons/collections/dictionary.h>
#include <package/package.h>
#include <initial_configuration/server_start.h>
#include <instruction_memory/process/process.h>
#include <instruction_memory/instructions/instructions.h>
#include <user_memory/real_memory.h>
#include <command/opcodes.h>
#include <initial_configuration/memory_config.h>
#include <pcb/pcb.h>
#include <page/page.h>

typedef struct {
    t_log* logger;
    int port;
    char* dict_key;
    t_conn* conn;
} t_thread;

void wait_in_every_port(t_conn* conn, t_log* logger);
void* wait_for_command(t_thread* thread_info);

#endif /* SRC_COMMAND_COMMAND_H_ */