#include <stdio.h>
#include <stdbool.h>
#include <pcb/pcb.h>
#include <stdlib.h>
#include <commons/string.h>
#include <cspecs/cspec.h>

context (pcb) {
    describe("Testing PCB") {
        it("Se serializa y se deserializa los registros sin perdida de información") {
            t_reg registers;
            registers.AX = 0;
            registers.BX = 2;
            registers.CX = 3;
            registers.DX = 20;
        	void* serialized_registers = serialize_registers(registers);
            t_reg deserialized_registers = deserialize_registers(serialized_registers);
            should_int(deserialized_registers.AX) be equal to (0);
            should_int(deserialized_registers.BX) be equal to (2);
            should_int(deserialized_registers.CX) be equal to (3);
            should_int(deserialized_registers.DX) be equal to (20);
            free(serialized_registers);
        } end
        it("Se serializa y se deserializa una pcb sin perdida de información para instruccion NORMAL") {
            t_pcb* pcb = malloc(sizeof(t_pcb));
            t_reg registers;
            registers.AX = 0;
            registers.BX = 2;
            registers.CX = 3;
            registers.DX = 20;
            pcb->pid = 2;
            pcb->tamanio = 20;
            pcb->programCounter = 5;
            pcb->prioridad = 2;
            pcb->nom_arch_inst = string_duplicate("archivo.pdf");
            pcb->estado = BLOCKED;
            pcb->registers = registers;
            pcb->params = NULL;
            pcb->instruccion = NORMAL;
            pcb->open_files = list_create();
        	void* serialized_pcb = serialize_pcb(pcb);
            t_pcb* deserialized_pcb = deserialize_pcb(serialized_pcb);
            should_int(deserialized_pcb->pid) be equal to (2);
            should_int(deserialized_pcb->tamanio) be equal to (20);
            should_int(deserialized_pcb->programCounter) be equal to (5);
            should_int(deserialized_pcb->prioridad) be equal to (2);
            should_string(deserialized_pcb->nom_arch_inst) be equal to ("archivo.pdf");
            should_int(deserialized_pcb->estado) be equal to (BLOCKED);
            should_int(deserialized_pcb->registers.AX) be equal to (0);
            should_int(deserialized_pcb->registers.BX) be equal to (2);
            should_int(deserialized_pcb->registers.CX) be equal to (3);
            should_int(deserialized_pcb->registers.DX) be equal to (20);
            should_int(deserialized_pcb->instruccion) be equal to (NORMAL);
            should_ptr(deserialized_pcb->params) be equal to (NULL);
            destroy_pcb(pcb);
            destroy_pcb(deserialized_pcb);
        } end
        it("Se serializa y se deserializa una pcb sin perdida de información para instruccion FINISH") {
            t_pcb* pcb = malloc(sizeof(t_pcb));
            t_reg registers;
            registers.AX = 0;
            registers.BX = 2;
            registers.CX = 3;
            registers.DX = 20;
            pcb->pid = 2;
            pcb->tamanio = 20;
            pcb->params = NULL;
            pcb->instruccion = FINISH;
            pcb->programCounter = 5;
            pcb->prioridad = 2;
            pcb->nom_arch_inst = string_duplicate("archivo.pdf");
            pcb->estado = BLOCKED;
            pcb->registers = registers;
            pcb->open_files = list_create();
        	void* serialized_pcb = serialize_pcb(pcb);
            t_pcb* deserialized_pcb = deserialize_pcb(serialized_pcb);
            should_int(deserialized_pcb->pid) be equal to (2);
            should_int(deserialized_pcb->tamanio) be equal to (20);
            should_int(deserialized_pcb->programCounter) be equal to (5);
            should_int(deserialized_pcb->prioridad) be equal to (2);
            should_string(deserialized_pcb->nom_arch_inst) be equal to ("archivo.pdf");
            should_int(deserialized_pcb->estado) be equal to (BLOCKED);
            should_int(deserialized_pcb->registers.AX) be equal to (0);
            should_int(deserialized_pcb->registers.BX) be equal to (2);
            should_int(deserialized_pcb->registers.CX) be equal to (3);
            should_int(deserialized_pcb->registers.DX) be equal to (20);
            should_int(deserialized_pcb->instruccion) be equal to (FINISH);
            should_ptr(deserialized_pcb->params) be equal to (NULL);
            destroy_pcb(pcb);
            destroy_pcb(deserialized_pcb);
        } end
        it("Se serializa y se deserializa una pcb sin perdida de información para instruccion WAIT") {
            t_pcb* pcb = malloc(sizeof(t_pcb));
            t_reg registers;
            registers.AX = 0;
            registers.BX = 2;
            registers.CX = 3;
            registers.DX = 20;
            pcb->pid = 2;
            pcb->tamanio = 20;
            pcb->instruccion = WAIT;
            pcb->params = string_duplicate("AR");
            pcb->programCounter = 5;
            pcb->prioridad = 2;
            pcb->nom_arch_inst = string_duplicate("archivo.pdf");
            pcb->estado = BLOCKED;
            pcb->registers = registers;
            pcb->open_files = list_create();
        	void* serialized_pcb = serialize_pcb(pcb);
            t_pcb* deserialized_pcb = deserialize_pcb(serialized_pcb);
            should_int(deserialized_pcb->pid) be equal to (2);
            should_int(deserialized_pcb->tamanio) be equal to (20);
            should_int(deserialized_pcb->programCounter) be equal to (5);
            should_int(deserialized_pcb->prioridad) be equal to (2);
            should_string(deserialized_pcb->nom_arch_inst) be equal to ("archivo.pdf");
            should_int(deserialized_pcb->estado) be equal to (BLOCKED);
            should_int(deserialized_pcb->registers.AX) be equal to (0);
            should_int(deserialized_pcb->registers.BX) be equal to (2);
            should_int(deserialized_pcb->registers.CX) be equal to (3);
            should_int(deserialized_pcb->registers.DX) be equal to (20);
            should_int(deserialized_pcb->instruccion) be equal to (WAIT);
            should_string(deserialized_pcb->params) be equal to ("AR");
            destroy_pcb(pcb);
            destroy_pcb(deserialized_pcb);
        } end
        it("Se serializa y se deserializa una pcb sin perdida de información para instruccion SIGNAL") {
            t_pcb* pcb = malloc(sizeof(t_pcb));
            t_reg registers;
            registers.AX = 0;
            registers.BX = 2;
            registers.CX = 3;
            registers.DX = 20;
            pcb->pid = 2;
            pcb->tamanio = 20;
            pcb->instruccion = SIGNAL;
            pcb->params = string_duplicate("RA");
            pcb->programCounter = 5;
            pcb->prioridad = 2;
            pcb->nom_arch_inst = string_duplicate("archivo.pdf");
            pcb->estado = BLOCKED;
            pcb->registers = registers;
            pcb->open_files = list_create();
        	void* serialized_pcb = serialize_pcb(pcb);
            t_pcb* deserialized_pcb = deserialize_pcb(serialized_pcb);
            should_int(deserialized_pcb->pid) be equal to (2);
            should_int(deserialized_pcb->tamanio) be equal to (20);
            should_int(deserialized_pcb->programCounter) be equal to (5);
            should_int(deserialized_pcb->prioridad) be equal to (2);
            should_string(deserialized_pcb->nom_arch_inst) be equal to ("archivo.pdf");
            should_int(deserialized_pcb->estado) be equal to (BLOCKED);
            should_int(deserialized_pcb->registers.AX) be equal to (0);
            should_int(deserialized_pcb->registers.BX) be equal to (2);
            should_int(deserialized_pcb->registers.CX) be equal to (3);
            should_int(deserialized_pcb->registers.DX) be equal to (20);
            should_int(deserialized_pcb->instruccion) be equal to (SIGNAL);
            should_string(deserialized_pcb->params) be equal to ("RA");
            destroy_pcb(pcb);
            destroy_pcb(deserialized_pcb);
        } end
        it("Se serializa y se deserializa una pcb sin perdida de información para instruccion FCLOSE") {
            t_pcb* pcb = malloc(sizeof(t_pcb));
            t_reg registers;
            registers.AX = 0;
            registers.BX = 2;
            registers.CX = 3;
            registers.DX = 20;
            pcb->pid = 2;
            pcb->tamanio = 20;
            pcb->instruccion = FCLOSE;
            pcb->params = string_duplicate("ARCHIVO");
            pcb->programCounter = 5;
            pcb->prioridad = 2;
            pcb->nom_arch_inst = string_duplicate("archivo.pdf");
            pcb->estado = BLOCKED;
            pcb->registers = registers;
            pcb->open_files = list_create();
        	void* serialized_pcb = serialize_pcb(pcb);
            t_pcb* deserialized_pcb = deserialize_pcb(serialized_pcb);
            should_int(deserialized_pcb->pid) be equal to (2);
            should_int(deserialized_pcb->tamanio) be equal to (20);
            should_int(deserialized_pcb->programCounter) be equal to (5);
            should_int(deserialized_pcb->prioridad) be equal to (2);
            should_string(deserialized_pcb->nom_arch_inst) be equal to ("archivo.pdf");
            should_int(deserialized_pcb->estado) be equal to (BLOCKED);
            should_int(deserialized_pcb->registers.AX) be equal to (0);
            should_int(deserialized_pcb->registers.BX) be equal to (2);
            should_int(deserialized_pcb->registers.CX) be equal to (3);
            should_int(deserialized_pcb->registers.DX) be equal to (20);
            should_int(deserialized_pcb->instruccion) be equal to (FCLOSE);
            should_string(deserialized_pcb->params) be equal to ("ARCHIVO");
            destroy_pcb(pcb);
            destroy_pcb(deserialized_pcb);
        } end
        it("Se serializa y se deserializa una pcb sin perdida de información para instruccion SLEEP") {
            t_pcb* pcb = malloc(sizeof(t_pcb));
            t_reg registers;
            registers.AX = 0;
            registers.BX = 2;
            registers.CX = 3;
            registers.DX = 20;
            pcb->pid = 2;
            pcb->tamanio = 20;
            pcb->instruccion = SLEEP;
            pcb->params = 3;
            pcb->programCounter = 5;
            pcb->prioridad = 2;
            pcb->nom_arch_inst = string_duplicate("archivo.pdf");
            pcb->estado = BLOCKED;
            pcb->registers = registers;
            pcb->open_files = list_create();
        	void* serialized_pcb = serialize_pcb(pcb);
            t_pcb* deserialized_pcb = deserialize_pcb(serialized_pcb);
            should_int(deserialized_pcb->pid) be equal to (2);
            should_int(deserialized_pcb->tamanio) be equal to (20);
            should_int(deserialized_pcb->programCounter) be equal to (5);
            should_int(deserialized_pcb->prioridad) be equal to (2);
            should_string(deserialized_pcb->nom_arch_inst) be equal to ("archivo.pdf");
            should_int(deserialized_pcb->estado) be equal to (BLOCKED);
            should_int(deserialized_pcb->registers.AX) be equal to (0);
            should_int(deserialized_pcb->registers.BX) be equal to (2);
            should_int(deserialized_pcb->registers.CX) be equal to (3);
            should_int(deserialized_pcb->registers.DX) be equal to (20);
            should_int(deserialized_pcb->instruccion) be equal to (SLEEP);
            should_int(deserialized_pcb->params) be equal to (3);
            destroy_pcb(pcb);
            destroy_pcb(deserialized_pcb);
        } end
        it("Se serializa y se deserializa una pcb sin perdida de información para instruccion SLEEP con archivos abiertos correctamente") {
            t_pcb* pcb = malloc(sizeof(t_pcb));
            t_reg registers;
            registers.AX = 0;
            registers.BX = 2;
            registers.CX = 3;
            registers.DX = 20;
            pcb->pid = 2;
            pcb->tamanio = 20;
            pcb->instruccion = SLEEP;
            pcb->params = 3;
            pcb->programCounter = 5;
            pcb->prioridad = 2;
            pcb->nom_arch_inst = string_duplicate("archivo.pdf");
            pcb->estado = BLOCKED;
            pcb->registers = registers;
            t_openf* open_file = malloc(sizeof(*open_file));
            open_file->file = string_duplicate("hola");
            open_file->seek = 1;
            t_openf* open_file2 = malloc(sizeof(*open_file2));
            open_file2->file = string_duplicate("hola2");
            open_file2->seek = 2;
            pcb->open_files = list_create();
            list_add(pcb->open_files, open_file);
            list_add(pcb->open_files, open_file2);
        	void* serialized_pcb = serialize_pcb(pcb);
            t_pcb* deserialized_pcb = deserialize_pcb(serialized_pcb);
            should_int(deserialized_pcb->pid) be equal to (2);
            should_int(deserialized_pcb->tamanio) be equal to (20);
            should_int(deserialized_pcb->programCounter) be equal to (5);
            should_int(deserialized_pcb->prioridad) be equal to (2);
            should_string(deserialized_pcb->nom_arch_inst) be equal to ("archivo.pdf");
            should_int(deserialized_pcb->estado) be equal to (BLOCKED);
            should_int(deserialized_pcb->registers.AX) be equal to (0);
            should_int(deserialized_pcb->registers.BX) be equal to (2);
            should_int(deserialized_pcb->registers.CX) be equal to (3);
            should_int(deserialized_pcb->registers.DX) be equal to (20);
            should_int(deserialized_pcb->instruccion) be equal to (SLEEP);
            should_int(deserialized_pcb->params) be equal to (3);
            t_openf* open_file_deserialized = list_get(deserialized_pcb->open_files, 0);
            should_string(open_file_deserialized->file) be equal to ("hola");
            should_int(open_file_deserialized->seek) be equal to (1);
            t_openf* open_file_deserialized2 = list_get(deserialized_pcb->open_files, 1);
            should_string(open_file_deserialized2->file) be equal to ("hola2");
            should_int(open_file_deserialized2->seek) be equal to (2);
            destroy_pcb(pcb);
            destroy_pcb(deserialized_pcb);
        } end
        it("Se serializa y se deserializa una pcb sin perdida de información para instruccion FOPEN") {
            t_pcb* pcb = malloc(sizeof(t_pcb));
            t_reg registers;
            registers.AX = 0;
            registers.BX = 2;
            registers.CX = 3;
            registers.DX = 20;
            pcb->pid = 2;
            pcb->tamanio = 20;
            pcb->instruccion = FOPEN;
            t_fopen* fopen = malloc(sizeof(*fopen));
            fopen->file_name = string_duplicate("hola");
            fopen->open_mode = string_duplicate("W");
            pcb->params = fopen;
            pcb->programCounter = 5;
            pcb->prioridad = 2;
            pcb->nom_arch_inst = string_duplicate("archivo.pdf");
            pcb->estado = BLOCKED;
            pcb->registers = registers;
            pcb->open_files = list_create();
        	void* serialized_pcb = serialize_pcb(pcb);
            t_pcb* deserialized_pcb = deserialize_pcb(serialized_pcb);
            should_int(deserialized_pcb->pid) be equal to (2);
            should_int(deserialized_pcb->tamanio) be equal to (20);
            should_int(deserialized_pcb->programCounter) be equal to (5);
            should_int(deserialized_pcb->prioridad) be equal to (2);
            should_string(deserialized_pcb->nom_arch_inst) be equal to ("archivo.pdf");
            should_int(deserialized_pcb->estado) be equal to (BLOCKED);
            should_int(deserialized_pcb->registers.AX) be equal to (0);
            should_int(deserialized_pcb->registers.BX) be equal to (2);
            should_int(deserialized_pcb->registers.CX) be equal to (3);
            should_int(deserialized_pcb->registers.DX) be equal to (20);
            should_int(deserialized_pcb->instruccion) be equal to (FOPEN);
            t_fopen* deserialized_fopen = (t_fopen*) deserialized_pcb->params;
            should_string(deserialized_fopen->file_name) be equal to ("hola");
            should_string(deserialized_fopen->open_mode) be equal to ("W");
            destroy_pcb(pcb);
            destroy_pcb(deserialized_pcb);
        } end
        it("Se serializa y se deserializa una pcb sin perdida de información para instruccion FTRUNCATE") {
            t_pcb* pcb = malloc(sizeof(t_pcb));
            t_reg registers;
            registers.AX = 0;
            registers.BX = 2;
            registers.CX = 3;
            registers.DX = 20;
            pcb->pid = 2;
            pcb->tamanio = 20;
            pcb->instruccion = FTRUNCATE;
            t_fchange* fchange = malloc(sizeof(*fchange));
            fchange->file_name = string_duplicate("holacomoestas");
            fchange->value = 10;
            pcb->params = fchange;
            pcb->programCounter = 5;
            pcb->prioridad = 2;
            pcb->nom_arch_inst = string_duplicate("archivo.pdf");
            pcb->estado = BLOCKED;
            pcb->registers = registers;
            pcb->open_files = list_create();
        	void* serialized_pcb = serialize_pcb(pcb);
            t_pcb* deserialized_pcb = deserialize_pcb(serialized_pcb);
            should_int(deserialized_pcb->pid) be equal to (2);
            should_int(deserialized_pcb->tamanio) be equal to (20);
            should_int(deserialized_pcb->programCounter) be equal to (5);
            should_int(deserialized_pcb->prioridad) be equal to (2);
            should_string(deserialized_pcb->nom_arch_inst) be equal to ("archivo.pdf");
            should_int(deserialized_pcb->estado) be equal to (BLOCKED);
            should_int(deserialized_pcb->registers.AX) be equal to (0);
            should_int(deserialized_pcb->registers.BX) be equal to (2);
            should_int(deserialized_pcb->registers.CX) be equal to (3);
            should_int(deserialized_pcb->registers.DX) be equal to (20);
            should_int(deserialized_pcb->instruccion) be equal to (FTRUNCATE);
            t_fchange* deserialized_fchange = (t_fchange*) deserialized_pcb->params;
            should_string(deserialized_fchange->file_name) be equal to ("holacomoestas");
            should_int(deserialized_fchange->value) be equal to (10);
            destroy_pcb(pcb);
            destroy_pcb(deserialized_pcb);
        } end
        it("Se serializa y se deserializa una pcb sin perdida de información para instruccion FSEEK") {
            t_pcb* pcb = malloc(sizeof(t_pcb));
            t_reg registers;
            registers.AX = 0;
            registers.BX = 2;
            registers.CX = 3;
            registers.DX = 20;
            pcb->pid = 2;
            pcb->tamanio = 20;
            pcb->instruccion = FSEEK;
            t_fchange* fchange = malloc(sizeof(*fchange));
            fchange->file_name = string_duplicate("comooo");
            fchange->value = 10;
            pcb->params = fchange;
            pcb->programCounter = 5;
            pcb->prioridad = 2;
            pcb->nom_arch_inst = string_duplicate("archivo.pdf");
            pcb->estado = BLOCKED;
            pcb->registers = registers;
            pcb->open_files = list_create();
        	void* serialized_pcb = serialize_pcb(pcb);
            t_pcb* deserialized_pcb = deserialize_pcb(serialized_pcb);
            should_int(deserialized_pcb->pid) be equal to (2);
            should_int(deserialized_pcb->tamanio) be equal to (20);
            should_int(deserialized_pcb->programCounter) be equal to (5);
            should_int(deserialized_pcb->prioridad) be equal to (2);
            should_string(deserialized_pcb->nom_arch_inst) be equal to ("archivo.pdf");
            should_int(deserialized_pcb->estado) be equal to (BLOCKED);
            should_int(deserialized_pcb->registers.AX) be equal to (0);
            should_int(deserialized_pcb->registers.BX) be equal to (2);
            should_int(deserialized_pcb->registers.CX) be equal to (3);
            should_int(deserialized_pcb->registers.DX) be equal to (20);
            should_int(deserialized_pcb->instruccion) be equal to (FSEEK);
            t_fchange* deserialized_fchange = (t_fchange*) deserialized_pcb->params;
            should_string(deserialized_fchange->file_name) be equal to ("comooo");
            should_int(deserialized_fchange->value) be equal to (10);
            destroy_pcb(pcb);
            destroy_pcb(deserialized_pcb);
        } end
        it("Se serializa y se deserializa una pcb sin perdida de información para instruccion FWRITE") {
            t_pcb* pcb = malloc(sizeof(t_pcb));
            t_reg registers;
            registers.AX = 0;
            registers.BX = 2;
            registers.CX = 3;
            registers.DX = 20;
            pcb->pid = 2;
            pcb->tamanio = 20;
            pcb->instruccion = FWRITE;
            t_fmodify* fmodify = malloc(sizeof(*fmodify));
            fmodify->file_name = string_duplicate("holas");
            fmodify->page = page_new(2, 10, 50);
            pcb->params = fmodify;
            pcb->programCounter = 5;
            pcb->prioridad = 2;
            pcb->nom_arch_inst = string_duplicate("archivo.pdf");
            pcb->estado = BLOCKED;
            pcb->registers = registers;
            pcb->open_files = list_create();
        	void* serialized_pcb = serialize_pcb(pcb);
            t_pcb* deserialized_pcb = deserialize_pcb(serialized_pcb);
            should_int(deserialized_pcb->pid) be equal to (2);
            should_int(deserialized_pcb->tamanio) be equal to (20);
            should_int(deserialized_pcb->programCounter) be equal to (5);
            should_int(deserialized_pcb->prioridad) be equal to (2);
            should_string(deserialized_pcb->nom_arch_inst) be equal to ("archivo.pdf");
            should_int(deserialized_pcb->estado) be equal to (BLOCKED);
            should_int(deserialized_pcb->registers.AX) be equal to (0);
            should_int(deserialized_pcb->registers.BX) be equal to (2);
            should_int(deserialized_pcb->registers.CX) be equal to (3);
            should_int(deserialized_pcb->registers.DX) be equal to (20);
            should_int(deserialized_pcb->instruccion) be equal to (FWRITE);
            t_fmodify* deserialized_fmodify = (t_fmodify*) deserialized_pcb->params;
            should_string(deserialized_fmodify->file_name) be equal to ("holas");
            should_int(deserialized_fmodify->page->pid) be equal to (2);
            should_int(deserialized_fmodify->page->page_number) be equal to (10);
            should_int(deserialized_fmodify->page->displacement) be equal to (50);
            destroy_pcb(pcb);
            destroy_pcb(deserialized_pcb);
        } end
        it("Se serializa y se deserializa una pcb sin perdida de información para instruccion FREAD") {
            t_pcb* pcb = malloc(sizeof(t_pcb));
            t_reg registers;
            registers.AX = 0;
            registers.BX = 2;
            registers.CX = 3;
            registers.DX = 20;
            pcb->pid = 2;
            pcb->tamanio = 20;
            pcb->instruccion = FREAD;
            t_fmodify* fmodify = malloc(sizeof(*fmodify));
            fmodify->file_name = string_duplicate("fread");
            fmodify->page = page_new(3, 20, 60);
            pcb->params = fmodify;
            pcb->programCounter = 5;
            pcb->prioridad = 2;
            pcb->nom_arch_inst = string_duplicate("archivo.pdf");
            pcb->estado = BLOCKED;
            pcb->registers = registers;
            pcb->open_files = list_create();
        	void* serialized_pcb = serialize_pcb(pcb);
            t_pcb* deserialized_pcb = deserialize_pcb(serialized_pcb);
            should_int(deserialized_pcb->pid) be equal to (2);
            should_int(deserialized_pcb->tamanio) be equal to (20);
            should_int(deserialized_pcb->programCounter) be equal to (5);
            should_int(deserialized_pcb->prioridad) be equal to (2);
            should_string(deserialized_pcb->nom_arch_inst) be equal to ("archivo.pdf");
            should_int(deserialized_pcb->estado) be equal to (BLOCKED);
            should_int(deserialized_pcb->registers.AX) be equal to (0);
            should_int(deserialized_pcb->registers.BX) be equal to (2);
            should_int(deserialized_pcb->registers.CX) be equal to (3);
            should_int(deserialized_pcb->registers.DX) be equal to (20);
            should_int(deserialized_pcb->instruccion) be equal to (FREAD);
            t_fmodify* deserialized_fmodify = (t_fmodify*) deserialized_pcb->params;
            should_string(deserialized_fmodify->file_name) be equal to ("fread");
            should_int(deserialized_fmodify->page->pid) be equal to (3);
            should_int(deserialized_fmodify->page->page_number) be equal to (20);
            should_int(deserialized_fmodify->page->displacement) be equal to (60);
            destroy_pcb(pcb);
            destroy_pcb(deserialized_pcb);
        } end
        it("Usando crear_pcb") {
            t_pcb* pcb = crear_pcb(1, string_duplicate("nombre"), 2, 3);
        	void* serialized_pcb = serialize_pcb(pcb);
            t_pcb* deserialized_pcb = deserialize_pcb(serialized_pcb);
            should_int(deserialized_pcb->pid) be equal to (1);
            should_int(deserialized_pcb->tamanio) be equal to (2);
            should_int(deserialized_pcb->programCounter) be equal to (0);
            should_int(deserialized_pcb->prioridad) be equal to (3);
            should_string(deserialized_pcb->nom_arch_inst) be equal to ("nombre");
            should_int(deserialized_pcb->estado) be equal to (NEW);
            should_int(deserialized_pcb->registers.AX) be equal to (0);
            should_int(deserialized_pcb->registers.BX) be equal to (0);
            should_int(deserialized_pcb->registers.CX) be equal to (0);
            should_int(deserialized_pcb->registers.DX) be equal to (0);
            should_int(deserialized_pcb->instruccion) be equal to (NORMAL);
            should_ptr(deserialized_pcb->params) be equal to (NULL);
            destroy_pcb(pcb);
            destroy_pcb(deserialized_pcb);
        } end
    } end
}