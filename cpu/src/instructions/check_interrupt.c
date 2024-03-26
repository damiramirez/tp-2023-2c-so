#include <instructions/check_interrupt.h>

int check_interrupt(t_pcb* pcb, int interrupt_fd, t_log* logger) {
    int bytes_read = 0;
    ioctl(interrupt_fd,FIONREAD,&bytes_read); 
    switch (bytes_read)
    {
        case FAIL_CONNECTION:
            return FAIL_CONNECTION;
        case NO_INTERRUPTION:
            return NO_INTERRUPTION;
    }
    int op_code = receive_op_code(interrupt_fd, logger);
    if (op_code != INTERRUPT_INSTRUCTION && op_code != INTERRUPT_FINISH_INSTRUCTION) {
        log_error(logger, "Incorrect op_code %d", op_code);
        return FAIL_CONNECTION;
    }
    t_pcb* pcb_to_interrupt = receive_pcb(interrupt_fd, logger);
    if (pcb_to_interrupt->pid != pcb->pid) {
        log_warning(logger, "Different pcb to interrupt. In CPU: %d, To Interrupt: %d", pcb_to_interrupt->pid, pcb->pid);
        destroy_pcb(pcb_to_interrupt);
        return check_interrupt(pcb, interrupt_fd, logger);
    }
    destroy_pcb(pcb_to_interrupt);
    pcb->instruccion = INTERRUPT_INSTRUCTION == op_code ? INTERRUPTED : INTERRUPT_FINISH;
    log_trace(logger, "Process Interrupted: %d", pcb->pid);
    return INTERRUPTION;
}