#ifndef SRC_PLANIFICADORES_CORTO_PLAZO_PRIORIDADES_PRIORIDADES_H_
#define SRC_PLANIFICADORES_CORTO_PLAZO_PRIORIDADES_PRIORIDADES_H_

#include <planificadores/planificador.h>
#include <planificadores/corto_plazo/common/common.h>

void* _min_priority(t_pcb* pcb1, t_pcb* pcb2);
void planificador_prioridades(t_planificador* info);

#endif /* SRC_PLANIFICADORES_CORTO_PLAZO_PRIORIDADES_PRIORIDADES_H_ */