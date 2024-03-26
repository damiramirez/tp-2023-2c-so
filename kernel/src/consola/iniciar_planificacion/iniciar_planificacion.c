#include "iniciar_planificacion.h"

void iniciar_planificacion(t_utils* utils, t_conn* conn) {
    if (!working) {
        working = true;
        iniciar_planificador_largo_plazo(utils->logger, conn->memory_socket);
        iniciar_planificador_corto_plazo(utils, conn);
    }
    log_info(utils->logger, "INICIO DE PLANIFICACIÓN");
}