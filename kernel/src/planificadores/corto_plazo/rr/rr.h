#ifndef SRC_PLANIFICADORES_CORTO_PLAZO_RR_RR_H_
#define SRC_PLANIFICADORES_CORTO_PLAZO_RR_RR_H_

#include <unistd.h>
#include <planificadores/planificador.h>
#include <planificadores/corto_plazo/common/common.h>

typedef struct {
    t_log* logger;
    int interrupt_socket;
    uint32_t pid; 
} t_rr;

void planificador_rr(t_planificador* info);

#endif /* SRC_PLANIFICADORES_CORTO_PLAZO_RR_RR_H_ */