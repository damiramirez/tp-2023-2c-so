#ifndef SRC_PLANIFICADORES_CORTO_PLAZO_CORTO_PLAZO_H_
#define SRC_PLANIFICADORES_CORTO_PLAZO_CORTO_PLAZO_H_

#include <initial_configuration/client_start.h>
#include <planificadores/corto_plazo/block/block.h>
#include <planificadores/resources/resources.h>
#include <planificadores/codigos_de_operacion.h>
#include <planificadores/corto_plazo/common/common.h>
#include <planificadores/corto_plazo/rr/rr.h>
#include <planificadores/corto_plazo/prioridades/prioridades.h>
#include <planificadores/corto_plazo/fifo/fifo.h>
#include <planificadores/corto_plazo/execute/execute.h>

#include <string.h>

void iniciar_planificador_corto_plazo(t_utils* utils, t_conn* conn);

#endif /* SRC_PLANIFICADORES_CORTO_PLAZO_CORTO_PLAZO_H_ */