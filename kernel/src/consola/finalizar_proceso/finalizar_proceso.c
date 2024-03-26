#include "finalizar_proceso.h"

void finalizar_proceso(uint32_t pid, t_log* logger, int socket_interrupt, int socket_memory) {
	t_pcb* pcb;
	if (estado_EXEC != NULL && estado_EXEC->pid == pid) {
		interrupt_finish_executing_process(socket_interrupt, logger);
		sem_wait(&finish_interrupted_process);
		pthread_mutex_lock(&mtx_execute_process);
		pcb = estado_EXEC;
		log_info(logger, "Finaliza el proceso %d - Motivo: MANUAL", pcb->pid);
		estado_EXEC = NULL;
		pthread_mutex_unlock(&mtx_execute_process);
		eliminar_proceso(pcb, socket_memory, logger);
		sem_post(&grd_mult);
		sem_post(&proceso_en_cola_ready);
		return;
	}

	bool _pcb_by_pid_in_list(t_pcb *pcb_in_list) {
        return pcb_in_list->pid == pid;
    };

	pcb = list_remove_by_condition(lista_estado_NEW, (void*) _pcb_by_pid_in_list);
	if (pcb != NULL) {
		log_info(logger, "Finaliza el proceso %d - Motivo: MANUAL", pcb->pid);
		eliminar_proceso(pcb, socket_memory, logger);
		return;
	}
	pthread_mutex_lock(&cola_ready);
	pcb = list_remove_by_condition(lista_estado_READY, (void*) _pcb_by_pid_in_list);
    pthread_mutex_unlock(&cola_ready);
	if (pcb != NULL) {
		log_info(logger, "Finaliza el proceso %d - Motivo: MANUAL", pcb->pid);
		sem_post(&grd_mult);
		eliminar_proceso(pcb, socket_memory, logger);
		return;
	}
	pthread_mutex_lock(&cola_exit);
	pcb = list_remove_by_condition(lista_estado_EXIT, (void*) _pcb_by_pid_in_list);
    pthread_mutex_unlock(&cola_exit);
	if (pcb != NULL) {
		log_info(logger, "Finaliza el proceso %d - Motivo: MANUAL", pcb->pid);
		eliminar_proceso(pcb, socket_memory, logger);
		return;
	}

	pthread_mutex_lock(&cola_sleep);
	pcb = list_find(lista_estado_SLEEP, (void*) _pcb_by_pid_in_list);
	if (pcb != NULL) {
		pcb->end_state = MANUAL;
		return;
	}
	pthread_mutex_unlock(&cola_sleep);

	pthread_mutex_lock(&cola_interrupt);
	pcb = list_find(lista_estado_INTERRUPT, (void*) _pcb_by_pid_in_list);
	if (pcb != NULL) {
		pcb->end_state = MANUAL;
		return;
	}
	pthread_mutex_unlock(&cola_interrupt);

	bool eliminado = false;
	void _pcb_by_pid_in_dict(char* _, t_block* block) {
		pcb = list_remove_by_condition(block->blocked_list, (void*) _pcb_by_pid_in_list);
		if (pcb != NULL) {
			eliminado = true;
			log_info(logger, "Finaliza el proceso %d - Motivo: MANUAL", pcb->pid);
			sem_post(&grd_mult);
			eliminar_proceso(pcb, socket_memory, logger);
		}
    };
	dictionary_iterator(colas_BLOCKED, (void*) _pcb_by_pid_in_dict);
	if (!eliminado) {
		printf("No fue encontrado el proceso con el pid %d\n", pid);
	}
};