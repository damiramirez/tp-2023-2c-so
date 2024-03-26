#ifndef FCB_H_INCLUDED
#define FCB_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <config/config.h>
#include <string.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/memory.h>
#include "initial_configuration/fs_config.h"
#include <dirent.h>

void create_fcb_file(char *fileName);
void print_fcb_list();
t_fcb *find_fcb_file(char *file_name);
void update_fcb(t_fcb *fcb);
void load_FCBs_from_directory(char *directory_path);

#endif