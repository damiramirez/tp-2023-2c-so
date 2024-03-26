#ifndef SRC_COMMAND_INTERRUPT_H_
#define SRC_COMMAND_INTERRUPT_H_

#include <config/config.h>
#include <package/package.h>
#include <initial_configuration/server_start.h>

int wait_for_interrupt_echo(t_utils* utils, t_conn* conn);

#endif /* SRC_COMMAND_INTERRUPT_H_ */