#ifndef SRC_PLANIFICADORES_RESOURCES_RESOURCES_H_
#define SRC_PLANIFICADORES_RESOURCES_RESOURCES_H_

#include <planificadores/planificador.h>
#include <planificadores/deadlock/deadlock.h>

void use_instance_of_resource(t_pcb* pcb, t_block* block, t_log* logger);
void signal_instance_of_resource(t_pcb* pcb, t_log* logger);
void wait_instance_of_resource(t_pcb* pcb, t_log* logger);

#endif /* SRC_PLANIFICADORES_RESOURCES_RESOURCES_H_ */