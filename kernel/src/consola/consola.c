#include "consola.h"

void consola_interactiva(t_utils* utils, t_conn* conn){
	printf("[1]-INICIAR_PROCESO [PATH] [SIZE] [PRIORIDAD]\n");
	printf("[2]-FINALIZAR_PROCESO [PID]\n");
	printf("[3]-DETENER_PLANIFICACION \n");
	printf("[4]-INICIAR_PLANIFICACION \n");
	printf("[5]-MULTIPROGRAMACION [VALOR]\n");
	printf("[6]-PROCESO_ESTADO \n");
	char* linea = readline(">");
	while (!string_is_empty(linea)) {
		char **parametros = string_split(linea, " ");
		free(linea);
		if(strcmp(parametros[0],"INICIAR_PROCESO")==0){
			iniciar_proceso(string_duplicate(parametros[1]), string_duplicate(parametros[2]), string_duplicate(parametros[3]), utils->logger, conn);
		}
		else if(strcmp(parametros[0],"FINALIZAR_PROCESO")==0) {
			char* param = string_duplicate(parametros[1]);
			finalizar_proceso(atoi(param), utils->logger, conn->cpu_interrupt_socket, conn->memory_socket);
			free(param);
		}
		else if(strcmp(parametros[0],"DETENER_PLANIFICACION")==0){
			detener_planificacion(utils->logger);
		}
		else if(strcmp(parametros[0],"INICIAR_PLANIFICACION")==0){
			iniciar_planificacion(utils, conn);
		}
		else if(strcmp(parametros[0],"MULTIPROGRAMACION")==0){
			cambiar_grado_multiprogramacion(parametros[1], utils->logger);
		}
		else if(strcmp(parametros[0],"PROCESO_ESTADO")==0){
			listar_procesos_por_estados(utils->logger);
		}
		else if(strcmp(parametros[0],"ECHO")==0){
			int number = atoi(parametros[1]);
			if (number == 0) {
				log_error(utils->logger, "Invalid CASE Input: %s", parametros[1]);
			} else {
				echo(number, conn, utils->logger);
			}
			free(parametros[1]);
		}
		else {
			printf("\nComando no reconocido, intente de nuevo corroborando espacios entre parametros y su sintaxis\n");
		}
		string_array_destroy(parametros);
		linea = readline(">");
		//I.cambiar la condicion de los ifs por un strcmp y el array de parametros para evitar problemas con los espacios luego del split.
		//II en vez de un encadenado de ifs, probar con un enum y un switch
	}
	rl_clear_history();
	free(linea);
}

