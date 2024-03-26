#ifndef SRC_CONSOLA_LISTAR_PROCESOS_LISTAR_PROCESOS_H_
#define SRC_CONSOLA_LISTAR_PROCESOS_LISTAR_PROCESOS_H_

#include <pcb/pcb.h>
#include "planificadores/planificador.h"

void listar_procesos_por_estados(t_log* logger);
void listar_procesos(char* name, t_list* list, t_log* logger, pthread_mutex_t mutex);

#endif /* SRC_CONSOLA_LISTAR_PROCESOS_LISTAR_PROCESOS_H_ */