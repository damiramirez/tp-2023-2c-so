#ifndef SRC_CONSOLA_FINALIZAR_PROCESO_FINALIZAR_PROCESO_H_
#define SRC_CONSOLA_FINALIZAR_PROCESO_FINALIZAR_PROCESO_H_

#include "planificadores/planificador.h"
#include <planificadores/largo_plazo/largo_plazo.h>
#include <planificadores/corto_plazo/common/common.h>

void finalizar_proceso(uint32_t pid, t_log* logger, int socket_interrupt, int socket_memory);

#endif /* SRC_CONSOLA_FINALIZAR_PROCESO_FINALIZAR_PROCESO_H_ */
