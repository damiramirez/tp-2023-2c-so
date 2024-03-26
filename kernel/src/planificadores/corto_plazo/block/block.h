#ifndef SRC_PLANIFICADORES_CORTO_PLAZO_BLOCK_BLOCK_H_
#define SRC_PLANIFICADORES_CORTO_PLAZO_BLOCK_BLOCK_H_

#include <planificadores/resources/resources.h>

typedef struct {
    t_log* logger;
    t_pcb* pcb;
    int socket_memory;
} t_interrupted;

void unblock_processes(t_log* logger);
void treat_interrupted_process(t_interrupted* interrupted_info);
void interrupt_process(t_pcb* pcb, int socket_memory, t_log* logger);

#endif /* SRC_PLANIFICADORES_CORTO_PLAZO_BLOCK_BLOCK_H_ */