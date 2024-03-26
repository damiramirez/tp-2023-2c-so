#ifndef SRC_INSTRUCTIONS_CHECK_INTERRUPT_H_
#define SRC_INSTRUCTIONS_CHECK_INTERRUPT_H_

#include <stdbool.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <instructions/commands.h>
#include <pcb/pcb.h>

enum Checks {
    FAIL_CONNECTION = -1,
    NO_INTERRUPTION = 0,
    INTERRUPTION = 1,
};

int check_interrupt(t_pcb* pcb, int interrupt_fd, t_log* logger);

#endif /* SRC_INSTRUCTIONS_CHECK_INTERRUPT_H_ */