#include "deadlock.h"

void analize_resource_deadlock(char* blocked_resource, t_block* blocked_block, t_log* logger) {
    log_info(logger, "ANÁLISIS DE DETECCIÓN DE DEADLOCK");
    t_dictionary* blocked_resources = dictionary_create();
    
    add_to_blocked_resources_from_dict(blocked_block->pids, blocked_resources);
    add_to_blocked_resources_from_list(blocked_block->blocked_list, blocked_resources);
    check_all_resources_of_blocked_processes(blocked_resources);

    if (check_if_all_processes_are_blocked(blocked_resources)) {
        void _show_process_deadlock(char* pid, t_list* list) {
            char* in_possession = get_string_of_resources_in_list(list);
            log_info(logger, "Deadlock detectado: %s - Recursos en posesión %s - Recurso requerido: %s", pid, in_possession, blocked_resource);
            free(in_possession);
        };
        dictionary_iterator(blocked_resources, (void*) _show_process_deadlock);
    }


    void _destroy_blocked_resources(t_list* list) {
        void _destroy_element(char* element) {
            free(element);
        };
        list_destroy_and_destroy_elements(list, (void*) _destroy_element);
    };

    dictionary_destroy_and_destroy_elements(blocked_resources, (void*) _destroy_blocked_resources);
}

char* get_string_of_resources_in_list(t_list* list) {
	int i = 0;
	char* resources = string_new();
	int cantElem = list->elements_count;
	for(i = 0; i <= cantElem - 1; i++){
		char* resource = list_get(list,i);
  		string_append_with_format(&resources, i == 0 ? "%s" :",%s", resource);
	}
	return resources;
}

bool check_if_all_processes_are_blocked(t_dictionary* blocked_resources) {
    bool all_blocked = true;
    void _check_blocked_processes(char* _, t_list* list) {
        if (list->elements_count == 0) {
            all_blocked = false;
        }
    };
    dictionary_iterator(blocked_resources, (void*) _check_blocked_processes);
    return all_blocked;
}

void check_all_resources_of_blocked_processes(t_dictionary* blocked_resources) {
    void _pcb_is_blocked(char* pid, t_list* list) {
        void _pcb_is_block_in_a_queue(char* resource, t_block* block) {
            bool _is_pcb(t_pcb* pcb_in_list) {
                return pcb_in_list->pid == atoi(pid); 
            };
            bool found_pid = list_any_satisfy(block->blocked_list, _is_pcb);
            if (found_pid) {
                list_add(list, string_duplicate(resource));
            }
        };
        dictionary_iterator(colas_BLOCKED, (void*) _pcb_is_block_in_a_queue);
    };
	dictionary_iterator(blocked_resources, (void*) _pcb_is_blocked);
}

void add_to_blocked_resources_from_dict(t_dictionary* original_dict, t_dictionary* new_dict) {
    void _add_to_blocked_resources_from_dict(char* pid, int _) {
        dictionary_put(new_dict, pid, list_create());
    };
    dictionary_iterator(original_dict, (void*) _add_to_blocked_resources_from_dict);
}

void add_to_blocked_resources_from_list(t_list* list, t_dictionary* new_dict) {
    void _add_to_blocked_resources_from_list(t_pcb* pcb) {
        char* pid = string_itoa(pcb->pid);
        if (!dictionary_has_key(new_dict, pid)) {
            dictionary_put(new_dict, pid, list_create());
        }
        free(pid);
    };
    list_iterate(list, (void*) _add_to_blocked_resources_from_list);
}
