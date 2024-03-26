#include "prioridades.h"

void* _min_priority(t_pcb* pcb1, t_pcb* pcb2) {
    return pcb1->prioridad <= pcb2->prioridad ? pcb1 : pcb2;
}

void planificador_prioridades(t_planificador* info) {
    while(1) {
        sem_wait(&proceso_en_cola_ready);
        if (!working) break;
        if (lista_estado_READY->elements_count != 0) {
            if (estado_EXEC == NULL) {
                pthread_mutex_lock(&cola_ready);
                t_pcb* pcb;
                if (lista_estado_READY->elements_count > 1) {
                    pcb = (t_pcb*) list_get_minimum(lista_estado_READY, (void*) _min_priority);
                    list_remove_element(lista_estado_READY, pcb);
                } else {
                    pcb = list_remove(lista_estado_READY, 0);
                }
                pthread_mutex_unlock(&cola_ready);
                execute_ready_process(pcb, info->utils->logger);
            } else {
                pthread_mutex_lock(&cola_ready);
                t_pcb* pcb = list_get(lista_estado_READY, lista_estado_READY->elements_count - 1);
                pthread_mutex_unlock(&cola_ready);
                if (estado_EXEC->prioridad > pcb->prioridad) {
                    interrupt_executing_process(info->conn->cpu_interrupt_socket, info->utils->logger);
                }
            }
        }
    }
    free(info);
}