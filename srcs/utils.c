#include "codexion.h"

long get_timestamp_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

// Checa, de forma segura (com mutex), se a simulação já terminou.
// Usada tanto por coder.c (loop principal) quanto por dongles.c
// (para não ficar esperando um dongle para sempre depois do fim).
int sim_is_over(t_sim *sim)
{
    int over;

    pthread_mutex_lock(&sim->end_lock);
    over = sim->simulation_over;
    pthread_mutex_unlock(&sim->end_lock);
    return (over);
}