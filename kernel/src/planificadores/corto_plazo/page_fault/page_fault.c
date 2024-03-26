#include "page_fault.h"

pthread_mutex_t *mutex_treat_page_fault;

void *treat_page_fault(t_pf *pf_info)
{
    pthread_mutex_lock(&cola_interrupt);
    list_add(lista_estado_INTERRUPT, pf_info->pcb);
    pthread_mutex_unlock(&cola_interrupt);
    t_pag *page = page_new(pf_info->pcb->pid, pf_info->pcb->params, 0);
    send_page(PAGE_FAULT_COMMAND, page, pf_info->memory_socket, pf_info->logger);
    free(page);
    pthread_mutex_lock(&mutex_treat_page_fault);
    int op_code = receive_op_code(pf_info->memory_socket, pf_info->logger);
    if (op_code != PAGE_FAULT_COMMAND)
    {
        log_warning(pf_info->logger, "Invalid op_code %d", op_code);
    }
    int *buffer = (int *)receive_buffer(pf_info->memory_socket, pf_info->logger);
    pthread_mutex_unlock(&mutex_treat_page_fault);
    if (*buffer != 0)
    {
        log_error(pf_info->logger, "Fail in response of page fault %d", *buffer);
    }
    free(buffer);
    pthread_mutex_lock(&cola_interrupt);
    list_remove_element(lista_estado_INTERRUPT, pf_info->pcb);
    pthread_mutex_unlock(&cola_interrupt);
    log_info(pf_info->logger, "PID: %d - Estado Anterior: %d - Estado Actual: %d", pf_info->pcb->pid, BLOCKED, READY);
    agregar_pcb_a_cola_READY(pf_info->pcb, pf_info->logger);
    free(pf_info);
}

void manage_page_fault(t_pcb *pcb, int memory_socket, t_log *logger)
{
    log_info(logger, "Page Fault PID: %d - Pagina: %d", pcb->pid, pcb->params);
    pcb->estado = BLOCKED;
    log_info(logger, "PID: %d - Estado Anterior: %d - Estado Actual: %d", pcb->pid, EXEC, BLOCKED);
    pthread_t interrupted_process_thread;
    t_pf *pf_info = malloc(sizeof(*pf_info));
    pf_info->pcb = pcb;
    pf_info->logger = logger;
    pf_info->memory_socket = memory_socket;
    pthread_create(&interrupted_process_thread, NULL, (void *)treat_page_fault, pf_info);
    pthread_detach(interrupted_process_thread);
    sem_post(&proceso_en_cola_ready);
}
