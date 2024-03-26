#include "planificador.h"

t_list *lista_estado_NEW;
t_list *lista_estado_READY;
t_dictionary *colas_BLOCKED;
t_list *lista_estado_EXIT;
t_pcb *estado_EXEC = NULL;

t_list *list_treat_page_fault;

t_list *lista_estado_SLEEP;
t_list *lista_estado_INTERRUPT;
t_dictionary *open_files_global_table;

pthread_mutex_t cola_ready;
pthread_mutex_t cola_exit;
pthread_mutex_t cola_sleep;
pthread_mutex_t cola_interrupt;
pthread_mutex_t cola_new;
pthread_mutex_t mtx_execute_process;
pthread_mutex_t open_files_global_table_mutex;

sem_t grd_mult;
sem_t proceso_en_cola_ready;
sem_t executing_process;
sem_t freed_resource;
sem_t process_in_exit;
pthread_mutex_t siguientePIDmutex;
uint32_t sig_PID;
uint32_t dispDeSalida;
sem_t process_in_new;
sem_t finish_interrupted_process;
char *algoritmo;
bool working = true;
int actual_grd_mult;

void iniciar_estructuras_planificadores(t_utils *config_kernel)
{

	sig_PID = 1;
	lista_estado_NEW = list_create();
	lista_estado_READY = list_create();
	lista_estado_EXIT = list_create();
	lista_estado_SLEEP = list_create();
	lista_estado_INTERRUPT = list_create();
	colas_BLOCKED = dictionary_create();
	open_files_global_table = dictionary_create();
	algoritmo = config_get_string_value(config_kernel->config, "ALGORITMO_PLANIFICACION");
	char **resources = config_get_array_value(config_kernel->config, "RECURSOS");
	char **resources_instances = config_get_array_value(config_kernel->config, "INSTANCIAS_RECURSOS");
	for (int i = 0; resources[i] != NULL; i++)
	{
		t_block *block = malloc(sizeof(*block));
		block->instances = atoi(resources_instances[i]);
		block->pids = dictionary_create();
		block->blocked_list = list_create();
		dictionary_put(colas_BLOCKED, resources[i], block);
	}
	string_array_destroy(resources);
	string_array_destroy(resources_instances);
	sem_init(&proceso_en_cola_ready, 0, 0);
	sem_init(&executing_process, 0, 0);
	sem_init(&freed_resource, 0, 0);
	sem_init(&process_in_exit, 0, 0);
	sem_init(&finish_interrupted_process, 0, 0);
	sem_init(&process_in_new, 0, 0);
	actual_grd_mult = config_get_int_value(config_kernel->config, "GRADO_MULTIPROGRAMACION_INI");
	sem_init(&grd_mult, 0, actual_grd_mult);
}

void terminar_estructuras_planificadores()
{
	working = false;
	sem_post(&executing_process);
	sem_post(&freed_resource);
	sem_post(&process_in_exit);
	sem_post(&process_in_new);
	sem_post(&proceso_en_cola_ready);
	void *_remove_pcb_in_list(t_pcb * pcb)
	{
		destroy_pcb(pcb);
	};
	list_destroy_and_destroy_elements(lista_estado_EXIT, _remove_pcb_in_list);
	list_destroy_and_destroy_elements(lista_estado_NEW, _remove_pcb_in_list);
	list_destroy_and_destroy_elements(lista_estado_READY, _remove_pcb_in_list);
	if (estado_EXEC != NULL)
		destroy_executing_process();
	void *_remove_block_in_dict(t_block * block)
	{
		list_destroy_and_destroy_elements(block->blocked_list, _remove_pcb_in_list);
		dictionary_destroy(block->pids);
		free(block);
	};
	dictionary_destroy_and_destroy_elements(colas_BLOCKED, _remove_block_in_dict);
	sem_destroy(&proceso_en_cola_ready);
	sem_destroy(&executing_process);
	sem_destroy(&grd_mult);
	sem_destroy(&freed_resource);
	sem_destroy(&process_in_exit);
	sem_destroy(&finish_interrupted_process);
	sem_destroy(&process_in_new);

	void *_remove_open_file(t_open_file * file)
	{
		list_destroy(file->locks);
		free(file);
	};
	dictionary_destroy_and_destroy_elements(open_files_global_table, _remove_open_file);
}

void destroy_executing_process()
{
	pthread_mutex_lock(&mtx_execute_process);
	destroy_pcb(estado_EXEC);
	estado_EXEC = NULL;
	pthread_mutex_unlock(&mtx_execute_process);
}

void modify_executing_process(t_pcb *pcb)
{
	pthread_mutex_lock(&mtx_execute_process);
	destroy_pcb(estado_EXEC);
	estado_EXEC = pcb;
	pthread_mutex_unlock(&mtx_execute_process);
}

void execute_ready_process(t_pcb *pcb, t_log *logger)
{
	log_info(logger, "PID: %d - Estado Anterior: %d - Estado Actual: %d", pcb->pid, READY, EXEC);
	pcb->estado = EXEC;
	pthread_mutex_lock(&mtx_execute_process);
	estado_EXEC = pcb;
	pthread_mutex_unlock(&mtx_execute_process);
	sem_post(&executing_process);
}

t_pcb *encontrar_proceso_por_PID(uint32_t pid)
{
	int _pcb_by_pid_in_list(t_pcb * pcb)
	{
		return pcb->pid == pid;
	};
	if (estado_EXEC->pid == pid)
		return estado_EXEC;

	t_pcb *pcb_en_new = list_find(lista_estado_NEW, (void *)_pcb_by_pid_in_list);
	if (pcb_en_new != NULL)
		return pcb_en_new;
	t_pcb *pcb_en_ready = list_find(lista_estado_READY, (void *)_pcb_by_pid_in_list);
	if (pcb_en_ready != NULL)
		return pcb_en_ready;
	t_pcb *pcb_en_exit = list_find(lista_estado_EXIT, (void *)_pcb_by_pid_in_list);
	if (pcb_en_exit != NULL)
		return pcb_en_exit;
	t_pcb *pcb_en_blocked = NULL;
	void _pcb_by_pid_in_dict(char *_, t_block *block)
	{
		t_pcb *pcb = list_find(block->blocked_list, (void *)_pcb_by_pid_in_list);
		if (pcb != NULL)
		{
			pcb_en_blocked = pcb;
		}
	};
	dictionary_iterator(colas_BLOCKED, (void *)_pcb_by_pid_in_dict);
	return pcb_en_blocked;
}

uint32_t obt_sig_PID()
{
	pthread_mutex_lock(&siguientePIDmutex);
	uint32_t nuevoPID = sig_PID + 1;
	pthread_mutex_unlock(&siguientePIDmutex);
	return nuevoPID;
}

void agregar_pcb_a_cola_READY(t_pcb *pcb, t_log *logger)
{
	int previous_state = pcb->estado;
	pthread_mutex_lock(&cola_ready);
	list_add(lista_estado_READY, pcb);
	pthread_mutex_unlock(&cola_ready);
	pcb->estado = READY;
	sem_post(&proceso_en_cola_ready);
	char *pids = get_string_of_pids_in_list(lista_estado_READY);
	log_info(logger, "Cola Ready %s: %s", algoritmo, pids);
	free(pids);
}

char *get_string_of_pids_in_list(t_list *list)
{
	int i = 0;
	char *pids = string_new();
	int cantElem = list->elements_count;
	for (i = 0; i <= cantElem - 1; i++)
	{
		t_pcb *pcb = list_get(list, i);
		char *pid = string_itoa(pcb->pid);
		string_append_with_format(&pids, i == 0 ? "%s" : ",%s", pid);
		free(pid);
	}
	return pids;
}

void close_lock(t_pcb *pcb, t_open_file *file, t_lock *lock, t_log *logger)
{
	if (lock->is_write_lock)
	{
		pthread_mutex_lock(&open_files_global_table_mutex);
		list_remove(file->locks, 0);
		pthread_mutex_unlock(&open_files_global_table_mutex);
		sem_destroy(&lock->write_locked);
		list_destroy(lock->participants);
		free(lock);
		if (file->locks->elements_count != 0)
		{
			t_lock *next_lock = list_get(file->locks, 0);
			if (next_lock->is_blocked)
			{
				log_debug(logger, "Se desbloqueo %d participants", next_lock->participants->elements_count);
				for (int i = 0; i < next_lock->participants->elements_count; i++)
				{
					sem_post(&next_lock->write_locked);
				}
			}
		}
	}
	else
	{
		int _pid_in_list(uint32_t pid)
		{
			return pid == pcb->pid;
		};
		list_remove_by_condition(lock->participants, _pid_in_list);
		if (lock->participants->elements_count == 0)
		{
			pthread_mutex_lock(&open_files_global_table_mutex);
			list_remove(file->locks, 0);
			pthread_mutex_unlock(&open_files_global_table_mutex);
			list_destroy(lock->participants);
			free(lock);
			if (file->locks->elements_count != 0)
			{
				t_lock *next_lock = list_get(file->locks, 0);
				log_debug(logger, "Se desbloqueo %d participants", next_lock->participants->elements_count);
				if (next_lock->is_blocked)
				{
					for (int i = 0; i < next_lock->participants->elements_count; i++)
					{
						sem_post(&next_lock->write_locked);
					}
				}
			}
		}
	}
}

void eliminar_proceso(t_pcb *pcb, int socket, t_log *logger)
{
	// Liberar instancias de archivos
	void *_free_files(t_openf * file)
	{
		pthread_mutex_lock(&open_files_global_table_mutex);
		t_open_file *open_file = dictionary_get(open_files_global_table, file->file);
		pthread_mutex_unlock(&open_files_global_table_mutex);

		bool _find_lock(t_lock * lock)
		{
			bool _contains_pid(uint32_t pid)
			{
				return pid == pcb->pid;
			}
			return list_any_satisfy(lock->participants, _contains_pid);
		}
		t_lock *lock = list_find(open_file->locks, _find_lock);
		close_lock(pcb, open_file, lock, logger);
	};
	list_iterate(pcb->open_files, _free_files);
	// Llamar a memoria para liberar
	send_pcb(FINISH_PROCESS, pcb, socket, logger);
	void *_free_instances_in_block(char *_, t_block *block)
	{
		char *pid = string_itoa(pcb->pid);
		bool has_resource_instances = dictionary_has_key(block->pids, pid);
		if (has_resource_instances)
		{
			block->instances += dictionary_remove(block->pids, pid);
			sem_post(&freed_resource);
		}
		free(pid);
	};

	dictionary_iterator(colas_BLOCKED, (void *)_free_instances_in_block);
	destroy_pcb(pcb);
}

void send_to_exit(t_pcb *pcb, t_log *logger, int end_state)
{
	sem_post(&grd_mult);
	pcb->estado = EXIT;
	pcb->end_state = end_state;
	pthread_mutex_lock(&cola_exit);
	list_add(lista_estado_EXIT, pcb);
	pthread_mutex_unlock(&cola_exit);
	log_info(logger, "PID: %d - Estado Anterior: %d - Estado Actual: %d", pcb->pid, EXEC, EXIT);
	sem_post(&proceso_en_cola_ready);
	sem_post(&process_in_exit);
}
// Pasarlos a tests!!!
// void prueba_elementos_cola_bloqueados(){
// 	printf("\n Llego aca\n");
// 	t_pcb* nuevoPCB = crear_pcb(1,"name", 2,2);
// 	list_add(colas_BLOCKED[0], nuevoPCB);
// 	nuevoPCB = crear_pcb(4,"name",2,2);
// 	list_add(colas_BLOCKED[0], nuevoPCB);
// 	nuevoPCB = crear_pcb(2,"name",2,2);
// 	list_add(colas_BLOCKED[1], nuevoPCB);
// 	nuevoPCB = crear_pcb(3,"name",2,2);
// 	list_add(colas_BLOCKED[2], nuevoPCB);
// 	uint32_t cantElem0 = list_size(colas_BLOCKED[0]);
// 	uint32_t cantElem1 = list_size(colas_BLOCKED[1]);
// 	uint32_t cantElem2 = list_size(colas_BLOCKED[2]);
// 	int i, j, k;
// 	printf("\n Cola BLOQUEADOS 0 = \n");
// 	for (i = 0; i <= cantElem0 - 1; i++){

// 		t_pcb* aMostrar = list_get(colas_BLOCKED[0],i);
// 		printf("ID de proceso = %d\n", aMostrar->pid);
// 	}
// 	printf("\n Cola BLOQUEADOS 1 = \n");
// 	for (j = 0; j <= cantElem1 - 1; j++){
// 		t_pcb* aMostrar = list_get(colas_BLOCKED[1],j);
// 		printf("ID de proceso = %d\n", aMostrar->pid);
// 	}
// 	printf("\n Cola BLOQUEADOS 2 = \n");
// 	for (k = 0; k <= cantElem2 - 1; k++){
// 		t_pcb* aMostrar = list_get(colas_BLOCKED[2],k);
// 		printf("ID de proceso = %d\n", aMostrar->pid);
// 	}
// }

void prueba_agregar_proceso_a_NEW()
{
	printf("\n Llego aca\n");
	t_pcb *nuevoPCB = crear_pcb(sig_PID, "name", 2, 2);
	list_add(lista_estado_NEW, nuevoPCB);
	sig_PID = obt_sig_PID();
	nuevoPCB = crear_pcb(sig_PID, "name", 2, 2);
	list_add(lista_estado_NEW, nuevoPCB);
	sig_PID = obt_sig_PID();
	nuevoPCB = crear_pcb(sig_PID, "name", 2, 2);
	list_add(lista_estado_NEW, nuevoPCB);
	sig_PID = obt_sig_PID();
	uint32_t cantElem = list_size(lista_estado_NEW);
	int i;
	for (i = 0; i <= cantElem - 1; i++)
	{
		t_pcb *aMostrar = list_get(lista_estado_NEW, i);
		printf("\n ID de proceso = %d\n", aMostrar->pid);
	}
}
