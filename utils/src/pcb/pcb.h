#ifndef SRC_PCB_PCB_H_
#define SRC_PCB_PCB_H_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <package/package.h>
#include "fs.h"
#include <commons/collections/list.h>
#include <page/page.h>

typedef struct {
    uint32_t AX;
    uint32_t BX;
    uint32_t CX;
    uint32_t DX;
} t_reg;

enum ESTADO {
    BLOCKED = 1,
    NEW = 2,
    EXIT = 3,
    EXEC = 4,
    READY = 5
};


enum INSTRUCCION {
    NORMAL = 0,
    FINISH = 1,
    SIGNAL = 2,
    WAIT = 3,
    SLEEP = 4,
    INTERRUPTED = 5,
    INTERRUPT_FINISH = 6,
    PAGE_FAULT = 8,
    FOPEN = 7,
    FCLOSE = 9,
    FSEEK = 10,
    FREAD = 11,
    FWRITE = 12,
    FTRUNCATE = 13
};

enum END_STATE {
    SUCCESS = 0,
    INVALID_RESOURCE  = 1,
    INVALID_WRITE = 2,
    MANUAL = 3
};

typedef struct pcb {
    uint32_t end_state; // Unicamente para el EXIT!
    uint32_t pid;
    uint32_t tamanio;
    uint32_t programCounter;
    uint32_t prioridad;
    char* nom_arch_inst;
    t_reg registers;
    uint32_t estado;
    uint32_t instruccion;
    void* params;
    t_list* open_files;
} t_pcb;

typedef struct open_file {
    char* file;
    int seek;
} t_openf;

int serialized_pcb_size(t_pcb* pcb);
t_reg create_empty_registers();
t_pcb* crear_pcb(uint32_t pid, char* name, uint32_t tamanio, uint32_t priority);
void* serialize_pcb(t_pcb* pcb);
void* serialize_registers(t_reg registers);
t_reg deserialize_registers(void* buffer);
t_pcb* deserialize_pcb(void* buffer);
void destroy_pcb(t_pcb* pcb);
void send_pcb(int op_code, t_pcb* pcb, int client_socket, t_log* logger);
t_pcb* receive_pcb(int client_socket, t_log* logger);
void destroy_params(t_pcb* pcb);
int serialized_open_file_size(t_openf* open_file);
void* serialize_open_file(t_openf* open_file);

#endif /* SRC_PCB_PCB_H_ */
