#include "block.h"

void unblock_processes(t_log* logger) {
    while (1)
    {
        sem_wait(&freed_resource);
        if (!working) break;
	    void* _send_blocked_pcb_to_ready(char* _, t_block* block) {
            if (block->instances >= 1 && block->blocked_list->elements_count >= 1) {
                t_pcb* pcb = list_remove(block->blocked_list, 0);
                use_instance_of_resource(pcb, block, logger);
	            log_info(logger, "PID: %d - Estado Anterior: %d - Estado Actual: %d", pcb->pid, BLOCKED, READY);
                agregar_pcb_a_cola_READY(pcb, logger);
            }
        };
        dictionary_iterator(colas_BLOCKED, _send_blocked_pcb_to_ready);
    }
}

void treat_interrupted_process(t_interrupted* interrupted_info) {
    if (interrupted_info->pcb->instruccion == SLEEP) {
        pthread_mutex_lock(&cola_sleep);
        list_add(lista_estado_SLEEP, interrupted_info->pcb);
        pthread_mutex_unlock(&cola_sleep);
        log_info(interrupted_info->logger, "PID: %d - Bloqueado por: SLEEP", interrupted_info->pcb->pid);
        sleep((int) interrupted_info->pcb->params);
    } else {
        pthread_mutex_lock(&cola_interrupt);
        list_add(lista_estado_INTERRUPT, interrupted_info->pcb);
        pthread_mutex_unlock(&cola_interrupt);
    }
    if (interrupted_info->pcb->instruccion == SLEEP) {
        pthread_mutex_lock(&cola_sleep);
        list_remove_element(lista_estado_SLEEP, interrupted_info->pcb);
        pthread_mutex_unlock(&cola_sleep);
    } else {
        pthread_mutex_lock(&cola_interrupt);
        list_remove_element(lista_estado_INTERRUPT, interrupted_info->pcb);
        pthread_mutex_unlock(&cola_interrupt);
    }

    if (interrupted_info->pcb->instruccion == SLEEP) {
        pthread_mutex_lock(&cola_sleep);
        if (interrupted_info->pcb->end_state == MANUAL) {
            pthread_mutex_unlock(&cola_sleep);
            log_info(interrupted_info->logger, "Finaliza el proceso %d - Motivo: MANUAL", interrupted_info->pcb->pid);
			sem_post(&grd_mult);
			eliminar_proceso(interrupted_info->pcb, interrupted_info->socket_memory, interrupted_info->logger);
            free(interrupted_info);
            return;
        }
        pthread_mutex_unlock(&cola_sleep);
    } else {
        pthread_mutex_lock(&cola_interrupt);
        if (interrupted_info->pcb->end_state == MANUAL) {
            pthread_mutex_unlock(&cola_interrupt);
			log_info(interrupted_info->logger, "Finaliza el proceso %d - Motivo: MANUAL", interrupted_info->pcb->pid);
			sem_post(&grd_mult);
			eliminar_proceso(interrupted_info->pcb, interrupted_info->socket_memory, interrupted_info->logger);
            free(interrupted_info);
            return;
        }
        pthread_mutex_unlock(&cola_interrupt);
    }
	log_info(interrupted_info->logger, "PID: %d - Estado Anterior: %d - Estado Actual: %d", interrupted_info->pcb->pid, BLOCKED, READY);
    agregar_pcb_a_cola_READY(interrupted_info->pcb, interrupted_info->logger);
    free(interrupted_info);
}

void interrupt_process(t_pcb* pcb, int socket_memory, t_log* logger) {
    pcb->estado = BLOCKED;
    log_info(logger, "PID: %d - Estado Anterior: %d - Estado Actual: %d", pcb->pid, EXEC, BLOCKED);
    pthread_t interrupted_process_thread;
    t_interrupted* interrupted_info = malloc(sizeof(*interrupted_info));
    interrupted_info->pcb = pcb;
    interrupted_info->logger = logger;
    interrupted_info->socket_memory = socket_memory;
    pthread_create(&interrupted_process_thread, NULL, (void*)treat_interrupted_process, interrupted_info);
    pthread_detach(interrupted_process_thread);
    sem_post(&proceso_en_cola_ready);
}