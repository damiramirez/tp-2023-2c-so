#include "multiprogramacion.h"

void *wait_grd_mlt()
{
    sem_wait(&grd_mult);
}

void cambiar_grado_multiprogramacion(char *valor, t_log *logger)
{
    int new_grade = atoi(valor);
    if (new_grade > actual_grd_mult)
    {
        int difference = new_grade - actual_grd_mult;
        for (int i = 0; i < difference; i++)
        {
            sem_post(&grd_mult);
        }
    }
    if (new_grade < actual_grd_mult)
    {
        int difference = actual_grd_mult - new_grade;
        for (int i = 0; i < difference; i++)
        {
            pthread_t wait_grd_mlt_thread;
            pthread_create(&wait_grd_mlt_thread, NULL, (void *)wait_grd_mlt, NULL);
            pthread_detach(wait_grd_mlt_thread);
        }
    }
    log_info(logger, "Grado Anterior: %d - Grado Actual: %d>", actual_grd_mult, valor);
    actual_grd_mult = valor;
}