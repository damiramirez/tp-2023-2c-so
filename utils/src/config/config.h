#ifndef SRC_CONFIG_CONFIG_H_
#define SRC_CONFIG_CONFIG_H_

#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <unistd.h>

#define NUMERO_DE_ARGUMENTOS_NECESARIOS 2

typedef struct {
	t_log* logger;
	t_config* config;
} t_utils;

t_utils* create_initial_config(int arg_count, char* argv[], char* logger_path, bool log_in_console, char* log_level);
t_log* create_logger(char* file, bool is_active_console, t_log_level log_level);
t_config* create_config(char* file, t_log* logger);
bool check_if_config_value_exists(t_utils* utils, char* key);
void utils_destroy(t_utils* utils);
void utils_destroy_with_connection(t_utils* utils, int connection);

#endif /* SRC_CONFIG_CONFIG_H_ */
