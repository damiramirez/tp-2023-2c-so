#include "listar_procesos.h"

void listar_procesos(char* name, t_list* list, t_log* logger, pthread_mutex_t mutex){
	pthread_mutex_lock(&mutex);
	char* pids = get_string_of_pids_in_list(list);
	pthread_mutex_unlock(&mutex);
	log_info(logger, "Estado: %s - Procesos: %s", name, pids);
	free(pids);
}

void listar_procesos_por_estados(t_log* logger){
	listar_procesos("SLEEP", lista_estado_SLEEP, logger, cola_sleep);
	listar_procesos("INTERRUPT", lista_estado_INTERRUPT, logger, cola_interrupt);
	listar_procesos("NEW", lista_estado_NEW, logger, cola_new);
	listar_procesos("READY", lista_estado_READY, logger, cola_ready);
	listar_procesos("EXIT", lista_estado_EXIT, logger, cola_exit);
	if (estado_EXEC == NULL) {
		printf("No hay proceso en EXECUTE\n");
	} else {
		log_info(logger, "Estado: EXECUTE - Proceso: %d", estado_EXEC->pid);
	}

	void _list_process_in_blocked(char* key, t_block* block) {
		char* state = string_new();
  		string_append_with_format(&state, " %s", key);
		char* pids = get_string_of_pids_in_list(block->blocked_list);
		log_info(logger, "Estado: %s - Procesos: %s", state, pids);
		free(pids);
		free(state);
    };

	dictionary_iterator(colas_BLOCKED, (void*) _list_process_in_blocked);
}