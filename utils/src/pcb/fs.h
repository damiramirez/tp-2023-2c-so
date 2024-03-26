#ifndef SRC_PCB_FS_H_
#define SRC_PCB_FS_H_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <page/page.h>

typedef struct {
    char* file_name;
    char* open_mode;
} t_fopen;

typedef struct {
    char* file_name;
    t_pag* page;
} t_fmodify;

typedef struct {
    char* file_name;
    int value;
} t_fchange;

int serialized_fopen_size(t_fopen* fopen);
void* serialize_fopen(t_fopen* fopen);
t_fopen* deserialize_fopen(void* buffer);
int serialized_fchange_size(t_fchange* fchange);
void* serialize_fchange(t_fchange* fchange);
t_fchange* deserialize_fchange(void* buffer);
int serialized_fmodify_size(t_fmodify* fmodify);
void* serialize_fmodify(t_fmodify* fmodify);
t_fmodify* deserialize_fmodify(void* buffer);

#endif /* SRC_PCB_FS_H_ */
