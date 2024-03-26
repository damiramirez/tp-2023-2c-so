#ifndef SRC_INSTRUCTIONS_FETCH_H_
#define SRC_INSTRUCTIONS_FETCH_H_

#include <package/package.h>
#include <pcb/pcb.h>
#define FETCH_INSTRUCTION 3

char* fetch(t_pcb* pcb, int memory_socket, t_log* logger);

#endif /* SRC_INSTRUCTIONS_FETCH_H_ */