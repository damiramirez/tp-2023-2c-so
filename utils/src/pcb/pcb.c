#include "pcb.h"

t_reg create_empty_registers() {
	t_reg registers;
	registers.AX = 0;
	registers.BX = 0;
	registers.CX = 0;
	registers.DX = 0;
	return registers;
}

t_pcb* crear_pcb(uint32_t pid, char* name, uint32_t tamanio, uint32_t priority) {
	t_pcb* nuevoPCB = malloc(sizeof(*nuevoPCB));
	nuevoPCB->pid = pid;
	nuevoPCB->tamanio = tamanio;
	nuevoPCB->programCounter = 0;
	nuevoPCB->prioridad = priority;
	nuevoPCB->nom_arch_inst = name;
	nuevoPCB->estado = NEW;
	nuevoPCB->registers=create_empty_registers();
	nuevoPCB->instruccion=NORMAL;
	nuevoPCB->params=NULL;
	nuevoPCB->end_state = NULL;
	nuevoPCB->open_files=list_create();
	return nuevoPCB;
}

void send_pcb(int op_code, t_pcb* pcb, int client_socket, t_log* logger) {
	t_package* package = create_empty_package(op_code);
    package->size = serialized_pcb_size(pcb);
    package->buffer = serialize_pcb(pcb);
    send_package(package, client_socket, logger);
}

t_pcb* receive_pcb(int client_socket, t_log* logger) {
	void* buffer = receive_buffer(client_socket, logger);
	if (buffer == NULL) {
		log_error(logger, "Error al recibir el buffer");
	}
   	return deserialize_pcb(buffer);
}

int serialized_pcb_size(t_pcb* pcb) {
	int arch_name_size = strlen(pcb->nom_arch_inst) + 1;
	int instruction_size = 0;
	switch (pcb->instruccion)
	{
		case FOPEN:
			instruction_size = serialized_fopen_size(pcb->params) + sizeof(int);
			break;
		case FSEEK:
		case FTRUNCATE:
			instruction_size = serialized_fchange_size(pcb->params) + sizeof(int);
			break;
		case FWRITE:
		case FREAD:
			instruction_size = serialized_fmodify_size(pcb->params) + sizeof(int);
			break;
		case WAIT:
		case SIGNAL:
		case FCLOSE:
			instruction_size = sizeof(char) * (strlen(pcb->params) + 1) + sizeof(int);
			break;
		case PAGE_FAULT:
		case SLEEP:
			instruction_size = sizeof(int);
			break;
	}
	int open_files_size = sizeof(int);
	for (int i = 0; i < pcb->open_files->elements_count; i++)
	{
		t_openf* open_file = list_get(pcb->open_files, i);
		open_files_size += serialized_open_file_size(open_file);
	}
	return 9*sizeof(uint32_t) + sizeof(char) * arch_name_size + 2*sizeof(int) + instruction_size + open_files_size;
}

int serialized_open_file_size(t_openf* open_file) {
	return sizeof(char) * (strlen(open_file->file) + 1) + 2 * sizeof(int);
}

void* serialize_open_file(t_openf* open_file) {
	void* buffer = malloc(serialized_open_file_size(open_file));
	int offset = 0;
	int file_name_size = strlen(open_file->file) + 1;
	memcpy(buffer + offset, &file_name_size, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer + offset, open_file->file, sizeof(char) * file_name_size);
	offset += sizeof(char) * file_name_size;
	memcpy(buffer + offset, &open_file->seek, sizeof(int));
	offset += sizeof(int);
	return buffer;
}

void* serialize_pcb(t_pcb* pcb) {
	int arch_name_size = strlen(pcb->nom_arch_inst) + 1;
	void* buffer = malloc(serialized_pcb_size(pcb));
	void* buffer_registers = serialize_registers(pcb->registers);
	int offset = 0;
	memcpy(buffer + offset, &pcb->instruccion, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	switch (pcb->instruccion)
	{
		case FOPEN:
			int fopen_size = serialized_fopen_size(pcb->params);
			memcpy(buffer + offset, &fopen_size, sizeof(int));
			offset += sizeof(int);
			void* buffer_fopen = serialize_fopen(pcb->params);
			memcpy(buffer + offset, buffer_fopen, fopen_size);
			offset += fopen_size;
			free(buffer_fopen);
			break;
		case FSEEK:
		case FTRUNCATE:
			int fchange_size = serialized_fchange_size(pcb->params);
			memcpy(buffer + offset, &fchange_size, sizeof(int));
			offset += sizeof(int);
			void* buffer_fchange = serialize_fchange(pcb->params);
			memcpy(buffer + offset, buffer_fchange, fchange_size);
			offset += fchange_size;
			free(buffer_fchange);
			break;
		case FWRITE:
		case FREAD:
			int fmodify_size = serialized_fmodify_size(pcb->params);
			memcpy(buffer + offset, &fmodify_size, sizeof(int));
			offset += sizeof(int);
			void* buffer_fmodify = serialize_fmodify(pcb->params);
			memcpy(buffer + offset, buffer_fmodify, fmodify_size);
			offset += fmodify_size;
			free(buffer_fmodify);
			break;
		case WAIT:
		case SIGNAL:
		case FCLOSE:
			int param_size = strlen(pcb->params) + 1;
			memcpy(buffer + offset, &param_size, sizeof(int));
			offset += sizeof(int);
			memcpy(buffer + offset, pcb->params, sizeof(char) * param_size);
			offset += sizeof(char) * param_size;
			break;
		case PAGE_FAULT:
		case SLEEP:
			memcpy(buffer + offset, &pcb->params, sizeof(int));
			offset += sizeof(int);
			break;
	}
	memcpy(buffer + offset, &pcb->pid, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer + offset, &pcb->tamanio, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer + offset, &pcb->programCounter, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer + offset, &pcb->prioridad, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer + offset, &arch_name_size, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer + offset, pcb->nom_arch_inst, sizeof(char) * arch_name_size);
	offset += sizeof(char) * arch_name_size;
	memcpy(buffer + offset, buffer_registers, 4 * sizeof(uint32_t));
	offset += 4 * sizeof(uint32_t);
	free(buffer_registers);
	memcpy(buffer + offset, &pcb->estado, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer + offset, &pcb->open_files->elements_count, sizeof(int));
	offset += sizeof(int);
	for (int i = 0; i < pcb->open_files->elements_count; i++)
	{
		t_openf* open_file = list_get(pcb->open_files, i);
		int size_serialized_open_file = serialized_open_file_size(open_file);
		void* buffer_open_file = serialize_open_file(open_file);
		memcpy(buffer + offset, buffer_open_file, size_serialized_open_file);
		offset += size_serialized_open_file;
		free(buffer_open_file);
	}
	
	return buffer;
}

void* serialize_registers(t_reg registers) {
	void* buffer = malloc(4*sizeof(uint32_t));
	int offset = 0;
	memcpy(buffer + offset, &registers.AX, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer + offset, &registers.BX, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer + offset, &registers.CX, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer + offset, &registers.DX, sizeof(uint32_t));
	return buffer;
}

t_reg deserialize_registers(void* buffer) {
	t_reg registers;
	int offset = 0;
	memcpy(&registers.AX, buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&registers.BX, buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&registers.CX, buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&registers.DX, buffer + offset, sizeof(uint32_t));
	return registers;
}

t_pcb* deserialize_pcb(void* buffer) {
	t_pcb* pcb = malloc(sizeof(*pcb));
	int offset = 0;
	memcpy(&pcb->instruccion, buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	switch (pcb->instruccion)
	{
		case FINISH:
		case NORMAL:
			pcb->params = NULL;
			break;
		case FOPEN:
			int fopen_size;
			memcpy(&fopen_size, buffer + offset, sizeof(int));
			offset += sizeof(int);
			void* buffer_fopen = malloc(fopen_size);
			memcpy(buffer_fopen, buffer + offset, fopen_size);
			pcb->params = deserialize_fopen(buffer_fopen);
			free(buffer_fopen);
			offset += fopen_size;
			break;
		case FTRUNCATE:
		case FSEEK:
			int fchange_size;
			memcpy(&fchange_size, buffer + offset, sizeof(int));
			offset += sizeof(int);
			void* buffer_fchange = malloc(fchange_size);
			memcpy(buffer_fchange, buffer + offset, fchange_size);
			pcb->params = deserialize_fchange(buffer_fchange);
			free(buffer_fchange);
			offset += fchange_size;
			break;
		case FWRITE:
		case FREAD:
			int fmodify_size;
			memcpy(&fmodify_size, buffer + offset, sizeof(int));
			offset += sizeof(int);
			void* buffer_fmodify = malloc(fmodify_size);
			memcpy(buffer_fmodify, buffer + offset, fmodify_size);
			pcb->params = deserialize_fmodify(buffer_fmodify);
			free(buffer_fmodify);
			offset += fmodify_size;
			break;
		case WAIT:
		case SIGNAL:
		case FCLOSE:
			int params_size;
			memcpy(&params_size, buffer + offset, sizeof(int));
			offset += sizeof(int);
			char* param = malloc(sizeof(char) * params_size);
			memcpy(param, buffer + offset, sizeof(char) * params_size);
			pcb->params=param;
			offset += params_size;
			break;
		case PAGE_FAULT:
		case SLEEP:
			int number;
			memcpy(&number, buffer + offset, sizeof(int));
			pcb->params = number;
			offset += sizeof(int);
			break;
	}
	memcpy(&pcb->pid, buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&pcb->tamanio, buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&pcb->programCounter, buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&pcb->prioridad, buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	int arch_name_size;
	memcpy(&arch_name_size, buffer + offset, sizeof(int));
	offset += sizeof(int);
	char* arch_name = malloc(sizeof(char) * arch_name_size);
	memcpy(arch_name, buffer + offset, sizeof(char) * arch_name_size);
	pcb->nom_arch_inst=arch_name;
	offset += arch_name_size;
	void* buffer_registers = malloc(4*sizeof(uint32_t));
	memcpy(buffer_registers, buffer + offset, 4 * sizeof(uint32_t));
	pcb->registers=deserialize_registers(buffer_registers);
	free(buffer_registers);
	offset += 4 * sizeof(uint32_t);
	memcpy(&pcb->estado, buffer + offset, sizeof(int));
	offset += sizeof(int);
	int open_file_count;
	memcpy(&open_file_count, buffer + offset, sizeof(int));
	offset += sizeof(int);
	pcb->open_files = list_create();
	for (int i = 0; i < open_file_count; i++)
	{
		t_openf* open_file = malloc(sizeof(*open_file));
		int open_file_name_size;
		memcpy(&open_file_name_size, buffer + offset, sizeof(int));
		offset += sizeof(int);
		char* open_file_name = malloc(sizeof(char) * open_file_name_size);
		memcpy(open_file_name, buffer + offset, sizeof(char) * open_file_name_size);
		offset += open_file_name_size;
		open_file->file = open_file_name;
		memcpy(&open_file->seek, buffer + offset, sizeof(int));
		offset += sizeof(int);
		list_add(pcb->open_files, open_file);
	}
	free(buffer);
	pcb->end_state = NULL;
	return pcb;
}

void destroy_params(t_pcb* pcb) {
	switch (pcb->instruccion)
	{
	case FOPEN:
		t_fopen* fopen_params = (t_fopen*) pcb->params;
		free(fopen_params->file_name);
		free(fopen_params->open_mode);
		free(pcb->params);
		break;
	case FSEEK:
	case FTRUNCATE:
		t_fchange* fchange_params = (t_fchange*) pcb->params;
		free(fchange_params->file_name);
		free(pcb->params);
		break;
	case FREAD:
	case FWRITE:
		t_fmodify* fmodify_params = (t_fmodify*) pcb->params;
		free(fmodify_params->file_name);
		free(fmodify_params->page);
		free(pcb->params);
		break;
	case WAIT:
	case SIGNAL:
	case FCLOSE:
		free(pcb->params);
		break;
	}
}

void destroy_pcb(t_pcb* pcb) {
	destroy_params(pcb);
	void* _remove_openf_in_list(t_openf* openf) {
		free(openf->file);
		free(openf);
    };
	list_destroy_and_destroy_elements(pcb->open_files, _remove_openf_in_list);
	free(pcb->nom_arch_inst);
	free(pcb);
}