#ifndef SRC_INSTRUCTIONS_DECODE_H_
#define SRC_INSTRUCTIONS_DECODE_H_

#define SEARCH_PAGE 8
#define PAGE_FRAME_COMMAND 9
#define PAGE_FAULT_COMMAND 10

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <pcb/pcb.h>
#include <page/page.h>
typedef struct {
    char* instruction;
    t_list* params;
    char* original_address;
} t_ins;

t_ins decode(t_pcb* pcb, char* instruction, int page_size, t_log* logger, int memory_socket);
t_pag* mmu_translate(uint32_t pid, char* logic_direction, int page_size, t_log* logger, int memory_socket);
void destroy_instruction(t_ins ins);

#endif /* SRC_INSTRUCTIONS_DECODE_H_ */