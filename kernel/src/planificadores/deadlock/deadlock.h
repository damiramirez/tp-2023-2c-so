#ifndef SRC_PLANIFICADORES_DEADLOCK_DEADLOCK_H_
#define SRC_PLANIFICADORES_DEADLOCK_DEADLOCK_H_

#include <planificadores/planificador.h>

char* get_string_of_resources_in_list(t_list* list);
void analize_resource_deadlock(char* blocked_resource, t_block* blocked_block, t_log* logger);
void check_all_resources_of_blocked_processes(t_dictionary* blocked_resources);
void add_to_blocked_resources_from_dict(t_dictionary* original_dict, t_dictionary* new_dict);
void add_to_blocked_resources_from_list(t_list* list, t_dictionary* new_dict);
char* get_string_of_resources_in_list(t_list* list);
bool check_if_all_processes_are_blocked(t_dictionary* blocked_resources);

#endif /* SRC_PLANIFICADORES_DEADLOCK_DEADLOCK_H_ */
