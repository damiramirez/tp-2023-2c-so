#include "rr.h"

int quantum;

void quantum_checker(t_rr* rr_info) {
    usleep(quantum * 1000);
    if (estado_EXEC != NULL && estado_EXEC->pid == rr_info->pid && lista_estado_READY->elements_count != 0) {
        interrupt_executing_process(rr_info->interrupt_socket, rr_info->logger);
        log_info(rr_info->logger, "PID: %d - Desalojado por fin de Quantum", rr_info->pid);
    } else {
        log_trace(rr_info->logger, "PID: %d - Ya no se esta ejecutando", rr_info->pid);
    }
    free(rr_info);
}

void planificador_rr(t_planificador* info) {
    quantum = config_get_int_value(info->utils->config, "QUANTUM");
    while(1) {
        sem_wait(&proceso_en_cola_ready);
        if (!working) break;
        if (estado_EXEC == NULL && lista_estado_READY->elements_count != 0) {
            pthread_mutex_lock(&cola_ready);
            t_pcb* pcb = list_remove(lista_estado_READY, 0);
            pthread_mutex_unlock(&cola_ready);
            execute_ready_process(pcb, info->utils->logger);
            t_rr* rr_info = malloc(sizeof(*rr_info));
            rr_info->interrupt_socket = info->conn->cpu_interrupt_socket;
            rr_info->logger = info->utils->logger;
            rr_info->pid = pcb->pid;
            pthread_t quantum_thread;
            pthread_create(&quantum_thread, NULL, (void*)quantum_checker, rr_info);
            pthread_detach(quantum_thread);
        }
    }
    free(info);
}
