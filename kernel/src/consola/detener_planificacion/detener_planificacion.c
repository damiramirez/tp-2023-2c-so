#include "detener_planificacion.h"

void detener_planificacion(t_log* logger) {
    working = false;
	sem_post(&executing_process);
	sem_post(&freed_resource);
	sem_post(&process_in_exit);
	sem_post(&process_in_new);
	sem_post(&proceso_en_cola_ready);
    log_info(logger, "PAUSA DE PLANIFICACIÓN");
};