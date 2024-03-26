#include "common.h"

void interrupt_executing_process(int socket, t_log* logger) {
    pthread_mutex_lock(&mtx_execute_process);
    send_pcb(INTERRUPT_INSTRUCTION, estado_EXEC, socket, logger);
    pthread_mutex_unlock(&mtx_execute_process);
}


void interrupt_finish_executing_process(int socket, t_log* logger) {
    pthread_mutex_lock(&mtx_execute_process);
    send_pcb(INTERRUPT_FINISH_INSTRUCTION, estado_EXEC, socket, logger);
    pthread_mutex_unlock(&mtx_execute_process);
}