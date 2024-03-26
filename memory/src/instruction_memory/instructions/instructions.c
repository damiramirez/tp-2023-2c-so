#include <instruction_memory/instructions/instructions.h>

t_list *get_instructions_from_file(t_log *logger, char *file_path)
{
    string_trim(&file_path);
    FILE *pseudo_code_file = fopen(file_path, "r");
    free(file_path);
    if (pseudo_code_file == NULL)
    {
        log_error(logger, "Error al leer el archivo pseudo codigo");

        return NULL;
    }

    char *line = NULL;
    char *instruction;
    size_t instruction_len = 0;
    t_list *instructions = list_create();

    while (getline(&line, &instruction_len, pseudo_code_file) != -1)
    {
        instruction = string_from_format("%s", line);
        string_trim(&instruction);
        list_add(instructions, instruction);
    }

    fclose(pseudo_code_file);
    if (line)
        free(line);

    return instructions;
}

char *fetch_next_instruction(int pid, int program_pointer, t_log* logger)
{
    t_process *current_process = search_process(pid);

    if(current_process == NULL) {
        log_error(logger, "Error al obtener el proceso en ejecucion");
        return "";
    }
    if(list_size(current_process->instructions_set) == program_pointer) {
        log_error(logger, "Todas las instrucciones del proceso con PID: %d han sido leidas", pid);
        return "";
    }

    return string_duplicate(list_get(current_process->instructions_set, program_pointer));
}