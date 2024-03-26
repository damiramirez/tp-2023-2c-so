#ifndef SRC_PLANIFICADORES_LARGO_PLAZO_LARGO_PLAZO_H_
#define SRC_PLANIFICADORES_LARGO_PLAZO_LARGO_PLAZO_H_

#include <planificadores/planificador.h>


typedef struct {
    t_log* logger;
    int socket_memory;
} t_exit;

void atender_procesos_en_EXIT(t_exit* exit_info);
void planificador_largo_plazo(t_log* logger);
void iniciar_planificador_largo_plazo(t_log* logger, int socket_memory);

#endif /* SRC_PLANIFICADORES_LARGO_PLAZO_LARGO_PLAZO_H_ */