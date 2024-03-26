#include "corto_plazo.h"

void iniciar_planificador_corto_plazo(t_utils* utils, t_conn* conn) {
    pthread_t hilo_planificador_corto_plazo;
    pthread_t hilo_execute;
    pthread_t hilo_blocked_processes;
    t_planificador* planificador_info = malloc(sizeof(*planificador_info));
    planificador_info->conn = conn;
    planificador_info->utils = utils;
    t_planificador* execute_info = malloc(sizeof(*execute_info));
    execute_info->conn = conn;
    execute_info->utils = utils;
    log_info(utils->logger,"Usando algoritmo: %s", algoritmo);
    if (strcmp(algoritmo,"FIFO")==0) {
        pthread_create(&hilo_planificador_corto_plazo, NULL, (void*)planificador_fifo, planificador_info);
    }
    if (strcmp(algoritmo,"RR")==0) {
        pthread_create(&hilo_planificador_corto_plazo, NULL, (void*)planificador_rr, planificador_info);
    }
    if (strcmp(algoritmo,"PRIORIDADES")==0) {
        pthread_create(&hilo_planificador_corto_plazo, NULL, (void*)planificador_prioridades, planificador_info);
    }
    pthread_detach(hilo_planificador_corto_plazo);
    pthread_create(&hilo_execute, NULL, (void*)execute_process, execute_info);
    pthread_detach(hilo_execute);
    pthread_create(&hilo_blocked_processes, NULL, (void*)unblock_processes, utils->logger);
    pthread_detach(hilo_blocked_processes);
}