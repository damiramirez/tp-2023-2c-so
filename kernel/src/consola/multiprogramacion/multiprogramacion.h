#ifndef SRC_CONSOLA_MULTIPROGRAMACION_MULTIPROGRAMACION_H_
#define SRC_CONSOLA_MULTIPROGRAMACION_MULTIPROGRAMACION_H_

#include "planificadores/planificador.h"

void* wait_grd_mlt();
void cambiar_grado_multiprogramacion(char* valor, t_log* logger);

#endif /* SRC_CONSOLA_MULTIPROGRAMACION_MULTIPROGRAMACION_H_ */