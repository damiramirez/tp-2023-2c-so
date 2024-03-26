#ifndef SRC_PLANIFICADORES_CORTO_PLAZO_COMMON_COMMON_H_
#define SRC_PLANIFICADORES_CORTO_PLAZO_COMMON_COMMON_H_

#include <initial_configuration/client_start.h>
#include <planificadores/planificador.h>

typedef struct {
    t_utils* utils;
    t_conn* conn;
} t_planificador;

void interrupt_executing_process(int socket, t_log* logger);
void interrupt_finish_executing_process(int socket, t_log* logger);

#endif /* SRC_PLANIFICADORES_CORTO_PLAZO_COMMON_COMMON_H_ */