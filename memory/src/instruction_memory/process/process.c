#include <instruction_memory/process/process.h>

t_list *active_processes;

int create_process(t_log *logger, t_pcb *pcb, t_list *swap_blocks)
{
    char *file_path = string_from_format("%s/%s.txt", memory_config.instructions_path, pcb->nom_arch_inst);
    t_list *instructions_set = get_instructions_from_file(logger, file_path);

    if (instructions_set == NULL)
        return -1;

    t_process *process = malloc(sizeof(*process));
    process->pid = pcb->pid;
    process->file_name = pcb->nom_arch_inst;
    process->bytes = pcb->tamanio;
    process->instructions_set = instructions_set;

    page_table_create(pcb, swap_blocks, logger);

    list_add(active_processes, process);

    return 1;
}

t_process *search_process(int pid)
{
    int _is_pid(t_process * process)
    {
        return process->pid == pid;
    };
    return (t_process *)list_find(active_processes, (void *)_is_pid);
}

static void _free_process(t_process *process)
{
    void _free_instruction(char *inst)
    {
        free(inst);
    };
    list_destroy_and_destroy_elements(process->instructions_set, (void *)_free_instruction);
    free(process);
};

void deallocate_process(uint32_t pid, int fs_socket, t_log *logger)
{
    int _is_pid(t_process * process)
    {
        return process->pid == pid;
    };
    t_page_table *page_table = search_page_table(pid);

    free_swap_blocks(page_table, fs_socket, logger);
    list_remove_element(page_tables, page_table);
    destroy_page_table(page_table, logger);
    list_remove_and_destroy_by_condition(active_processes, (void *)_is_pid, (void *)_free_process);
}