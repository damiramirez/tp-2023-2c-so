#include "iniciar_proceso.h"

uint32_t sig_PID;

void iniciar_proceso(char *source, char *size, char *priority, t_log *logger, t_conn *conn)
{
	t_pcb *pcbCreado = crear_proceso(source, size, priority);
	free(size);
	free(priority);
	avisar_a_memoria_nuevo_proceso(pcbCreado, logger, conn);
	agregar_pcb_a_cola_NEW(pcbCreado, logger);
}

void avisar_a_memoria_nuevo_proceso(t_pcb *pcb, t_log *logger, t_conn *conn)
{
	t_package *nuevoPaquete = create_empty_package(CREATE_PROCESS);
	void *buffer = serialize_pcb(pcb);
	nuevoPaquete->buffer = buffer;
	nuevoPaquete->size = serialized_pcb_size(pcb);
	send_package(nuevoPaquete, conn->memory_socket, logger);
	int op_code = receive_op_code(conn->memory_socket, logger);
	if (op_code != PROCESS_OK)
		log_warning(logger, "Received incorrect message %d", op_code);
	int *result = receive_buffer(conn->memory_socket, logger);
	log_debug(logger, "Result of creating process %d", *result);
	free(result);
};

void consultar_con_memoria_espacio_disp() {}

t_pcb *crear_proceso(char *source, char *tamanio, char *prioridad)
{
	consultar_con_memoria_espacio_disp();
	t_pcb *nuevoPCB = crear_pcb(sig_PID, source, atoi(tamanio), atoi(prioridad));
	return nuevoPCB;
}

void agregar_pcb_a_cola_NEW(t_pcb *pcb, t_log *logger)
{
	pthread_mutex_lock(&cola_new);
	list_add(lista_estado_NEW, pcb);
	pthread_mutex_unlock(&cola_new);
	log_info(logger, "Se crea el proceso %d en NEW", pcb->pid);
	sig_PID = obt_sig_PID();
	sem_post(&process_in_new);
}
