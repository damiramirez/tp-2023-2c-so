#include <instructions/fetch.h>

char* fetch(t_pcb* pcb, int memory_socket, t_log* logger) {
    int ins = FETCH_INSTRUCTION;
    send_pcb(FETCH_INSTRUCTION, pcb, memory_socket, logger);
    int op_code = receive_op_code(memory_socket, logger);
    if (op_code != FETCH_INSTRUCTION) log_warning(logger, "op_code no esperado %d", op_code);
    char* instruction = receive_buffer(memory_socket, logger);
    if (strcmp(instruction, "")==0) {
        log_error(logger, "Fail fetching instruction");
        return NULL;
    }
    log_info(logger, "PID: %d - FETCH - Program Counter: %d", pcb->pid, pcb->programCounter);
    log_debug(logger, "Instruccion: %s", instruction);
    return instruction;
}