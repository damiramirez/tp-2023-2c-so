#include "resources.h"

void use_instance_of_resource(t_pcb* pcb, t_block* block, t_log* logger) {
    block->instances--;
    char* pid = string_itoa(pcb->pid);
    int instances_of_pcb = 1;
    bool have_instances_already = dictionary_has_key(block->pids, pid);
    if (have_instances_already) {
        instances_of_pcb = dictionary_get(block->pids, pid);
        instances_of_pcb++;
    }
    dictionary_put(block->pids, pid, instances_of_pcb);
    free(pid);
}

void wait_instance_of_resource(t_pcb* pcb, t_log* logger) {
    t_block* block = dictionary_get(colas_BLOCKED, pcb->params);
    if (block->instances > 0) {
        use_instance_of_resource(pcb, block, logger);
        modify_executing_process(pcb);
        sem_post(&executing_process);
    } else {
        destroy_executing_process();
        pcb->estado = BLOCKED;
        list_add(block->blocked_list, pcb);
        log_info(logger, "PID: %d - Estado Anterior: %d - Estado Actual: %d", pcb->pid, EXEC, BLOCKED);
        log_info(logger, "PID: %d - Bloqueado por: %s", pcb->pid, pcb->params);
        analize_resource_deadlock(pcb->params, block, logger);
        sem_post(&proceso_en_cola_ready);
    }
    log_info(logger, "PID: %d - Wait: %s - Instancias: %d", pcb->pid, pcb->params, block->instances);
}

void signal_instance_of_resource(t_pcb* pcb, t_log* logger) {
    t_block* block = dictionary_get(colas_BLOCKED, pcb->params);
    char* pid = string_itoa(pcb->pid);
    bool have_instances_already = dictionary_has_key(block->pids, pid);
    if (have_instances_already) {
        block->instances++;
        int instances_of_pcb = dictionary_get(block->pids, pid);
        instances_of_pcb--;
        if (instances_of_pcb == 0) {
            dictionary_remove(block->pids, pid);
        } else {
            dictionary_put(block->pids, pid, instances_of_pcb);
        }
        sem_post(&freed_resource);
        modify_executing_process(pcb);
        sem_post(&executing_process);
        log_info(logger, "PID: %d - Signal: %s - Instancias: %d", pcb->pid, (char*) pcb->params, block->instances);
    } else {
        log_info(logger, "PID: %d - Signal: %s - Instancias: %d", pcb->pid, (char*) pcb->params, block->instances);
        destroy_executing_process();
        send_to_exit(pcb, logger, INVALID_RESOURCE);
    }
    free(pid);
}