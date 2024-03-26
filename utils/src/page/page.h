#ifndef SRC_PAGE_PAGE_H_
#define SRC_PAGE_PAGE_H_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <package/package.h>

typedef struct
{
    int page_number;
    int displacement;
    uint32_t pid;
} t_pag;

typedef struct
{
    int page_number;
    int displacement;
    uint32_t pid;
    uint32_t register_value;
} t_mov_out;

typedef struct
{
    int page_number;
    int displacement;
    uint32_t pid;
    void *register_value;
    int size;
} t_mov_out_fs;

typedef struct
{
    int swap_block;
    int page_info_size;
    void *page_content;
} t_pag_swap;

t_pag *page_new(uint32_t pid, int page_number, int displacement);
t_pag_swap *page_for_swap(int swap_block, int page_info_size, void *page_content);
t_mov_out *page_for_mov_out(uint32_t pid, int page_number, int displacement, uint32_t register_value);
t_mov_out_fs *page_for_mov_out_fs(uint32_t pid, int page_number, int displacement, void *register_value, int size);
void send_page_for_swap(int op_code, t_pag_swap *page, int client_socket, t_log *logger);
void send_page_for_mov_out(int op_code, t_mov_out *page, int client_socket, t_log *logger);
void send_page_for_mov_out_fs(int op_code, t_mov_out_fs *page, int client_socket, t_log *logger);
void send_page(int op_code, t_pag *page, int client_socket, t_log *logger);
void destroy_page(t_pag *page);
void destroy_page_for_mov_out(t_mov_out *page);
void destroy_page_for_mov_out_fs(t_mov_out_fs *page);
void destroy_page_for_swap(t_pag_swap *page);
t_pag *receive_page(int client_socket, t_log *logger);
t_mov_out *receive_page_for_mov_out(int client_socket, t_log *logger);
t_mov_out_fs *receive_page_for_mov_out_fs(int client_socket, t_log *logger);
t_pag_swap *receive_page_for_swap(int client_socket, t_log *logger);
int size_of_page(void);
t_pag *deserialize_page(void *buffer);
t_mov_out_fs *deserialize_page_for_mov_out_fs(void *buffer);
t_pag_swap *deserialize_page_for_swap(void *buffer);
void *serialize_page(t_pag *page);
void *serialize_page_for_mov_out_fs(t_mov_out_fs *page);
void *serialize_page_for_swap(t_pag_swap *page);

#endif /* SRC_PAGE_PAGE_H_ */
