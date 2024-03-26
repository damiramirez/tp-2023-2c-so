#include <consola/consola.h>
#include <planificadores/largo_plazo/largo_plazo.h>
#include <planificadores/corto_plazo/corto_plazo.h>

#define LOGS_KERNEL "kernel.log"

int main(int argc, char* argv[]) {
	t_utils* utils = create_initial_config(argc, argv, LOGS_KERNEL, false, argv[2]);
	if (utils == NULL) return EXIT_FAILURE;

	iniciar_estructuras_planificadores(utils);

	t_conn* conn = connect_to_modules(utils);
	if (conn == NULL) return EXIT_FAILURE;

	iniciar_planificador_largo_plazo(utils->logger, conn->memory_socket);
	iniciar_planificador_corto_plazo(utils, conn);
	consola_interactiva(utils, conn);

	log_info(utils->logger, "Finalizando Kernel");
	terminar_estructuras_planificadores();
	close_conn(conn);
    utils_destroy(utils);
    return 0;
}
