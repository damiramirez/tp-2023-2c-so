#include <instructions/decode.h>

t_ins decode(t_pcb* pcb, char* instruction, int page_size, t_log* logger, int memory_socket) {
    char** formatted_ins = string_split(instruction, " ");
    free(instruction);
    t_ins ins;
    ins.instruction = string_duplicate(formatted_ins[0]);
    ins.params = list_create();
    for (int i = 1; i < string_array_size(formatted_ins); i++)
    {
        list_add(ins.params, string_duplicate(formatted_ins[i]));
    }
    if (strcmp(ins.instruction, "MOV_IN") == 0 || strcmp(ins.instruction, "F_READ")==0 || strcmp(ins.instruction, "F_WRITE")==0) {
        char* logic_direction = list_get(ins.params, 1);
        t_pag* physical_direction = mmu_translate(pcb->pid, logic_direction, page_size, logger, memory_socket);
        destroy_params(pcb);
        if (physical_direction->pid == 0) {
            pcb->instruccion = PAGE_FAULT;
            pcb->params = physical_direction->page_number;
            free(physical_direction);
        } else {
            pcb->instruccion = NORMAL;
            ins.original_address = logic_direction;
            list_replace(ins.params, 1, physical_direction);
        }
    } else if (strcmp(ins.instruction, "MOV_OUT") == 0) {
        char* logic_direction = list_get(ins.params, 0);
        t_pag* physical_direction = mmu_translate(pcb->pid, logic_direction, page_size, logger, memory_socket);
        destroy_params(pcb);
        if (physical_direction->pid == 0) {
            pcb->instruccion = PAGE_FAULT;
            pcb->params = physical_direction->page_number;
            free(physical_direction);
        } else {
            pcb->instruccion = NORMAL;
            ins.original_address = logic_direction;
            list_replace(ins.params, 0, physical_direction);
        }
    }
    string_array_destroy(formatted_ins);
    return ins;
}

t_pag* mmu_translate(uint32_t pid, char* logic_direction, int page_size, t_log* logger, int memory_socket) {
    t_pag* physical_direction = malloc(sizeof(*physical_direction));
    int int_logic_direction = atoi(logic_direction);
    physical_direction->page_number = (int) floor(int_logic_direction / page_size);
    physical_direction->displacement = int_logic_direction - physical_direction->page_number * page_size;
    physical_direction->pid = pid;
    send_page(8, physical_direction, memory_socket, logger);
    int op_code = receive_op_code(memory_socket, logger);
    int* frame = (int*) receive_buffer(memory_socket, logger);
    switch (op_code)
    {
    case PAGE_FAULT_COMMAND:
        physical_direction->pid = 0;
        break;
    case PAGE_FRAME_COMMAND:
        log_info(logger, "PID: %d - OBTENER MARCO - Página: %d - Marco: %d", pid, physical_direction->page_number, *frame);
        break;
    default:
        log_warning(logger, "Ha llegado un opCode distinto al esperado para el page fault: %d", op_code);
        break;
    }
    free(frame);
    return physical_direction;
}

void destroy_instruction(t_ins ins) {
    void* _destroy_element(void* element) {
        free(element);
    };
    list_destroy_and_destroy_elements(ins.params, (void*) _destroy_element);
    free(ins.instruction);
}