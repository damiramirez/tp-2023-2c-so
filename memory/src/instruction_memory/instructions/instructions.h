#ifndef SRC_INSTRUCTION_MEMORY_INSTRUCTION_INSTRUCTION_H_
#define SRC_INSTRUCTION_MEMORY_INSTRUCTION_INSTRUCTION_H_

#include <commons/string.h>
#include <commons/collections/list.h>
#include <initial_configuration/memory_config.h>
#include <instruction_memory/process/process.h>

char* fetch_next_instruction(int pid, int program_pointer, t_log* logger);
t_list *get_instructions_from_file(t_log *logger, char *file_name);

#endif /* SRC_INSTRUCTION_MEMORY_INSTRUCTION_INSTRUCTION_H_*/