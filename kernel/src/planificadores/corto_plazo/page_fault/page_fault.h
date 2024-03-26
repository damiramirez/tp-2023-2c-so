#ifndef SRC_PLANIFICADORES_CORTO_PLAZO_PAGE_FAULT_PAGE_FAULT_H_
#define SRC_PLANIFICADORES_CORTO_PLAZO_PAGE_FAULT_PAGE_FAULT_H_

#include <planificadores/planificador.h>
#include <page/page.h>
#include <commons/collections/list.h>

typedef struct
{
    t_log *logger;
    t_pcb *pcb;
    int memory_socket;
} t_pf;

void manage_page_fault(t_pcb *pcb, int memory_socket, t_log *logger);

#endif /* SRC_PLANIFICADORES_CORTO_PLAZO_PAGE_FAULT_PAGE_FAULT_H_ */