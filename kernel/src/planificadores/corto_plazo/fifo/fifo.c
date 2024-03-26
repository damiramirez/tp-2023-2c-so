#include "fifo.h"

void planificador_fifo(t_planificador* info) {
    while(1) {
        sem_wait(&proceso_en_cola_ready);
        if (!working) break;
        if (estado_EXEC == NULL && lista_estado_READY->elements_count != 0) {
            pthread_mutex_lock(&cola_ready);
            t_pcb* pcb = list_remove(lista_estado_READY, 0);
            pthread_mutex_unlock(&cola_ready);
            execute_ready_process(pcb, info->utils->logger);
        }
    }
    free(info);
}