#include <config/config.h>

t_log* create_logger(char* file, bool is_active_console, t_log_level log_level)
{
	return log_create(file, "client", is_active_console, log_level);
}

t_config* create_config(char* file, t_log* logger)
{
	t_config* nuevo_config = config_create(file);
	if (nuevo_config == NULL) {
		log_error(logger, "No se encontro el archivo en %s", file);
		return NULL;
	}
	return nuevo_config;
}

t_utils* create_initial_config(int arg_count, char* argv[], char* logger_path, bool log_in_console, char* log_level)
{
	t_utils* utils = malloc(sizeof(t_utils));
	utils->logger = create_logger(logger_path, log_in_console, atoi(log_level));
	if (utils->logger == NULL) return NULL;
	if (arg_count == NUMERO_DE_ARGUMENTOS_NECESARIOS) {
		log_error(utils->logger, "Cantidad de argumentos invalida.");
	    log_destroy(utils->logger);
	    free(utils);
	    return NULL;
	}

	utils->config = create_config(argv[1], utils->logger);
	if (utils->config == NULL) {
		log_destroy(utils->logger);
		free(utils);
		return NULL;
	}
	return utils;
}

void utils_destroy(t_utils* utils) {
	log_destroy(utils->logger);
	config_destroy(utils->config);
	free(utils);
}

void utils_destroy_with_connection(t_utils* utils, int connection) {
	utils_destroy(utils);
	close(connection);
}

bool check_if_config_value_exists(t_utils* utils, char* key)
{
	if (!config_has_property(utils->config, key)) {
		log_error(utils->logger, "No se encontro la propiedad %s en el archivo de config", key);
		return false;
	}
	return true;
}
