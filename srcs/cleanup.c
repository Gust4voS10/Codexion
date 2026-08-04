#include "codexion.h"

static void destroy_wait_queue(t_sim *sim, t_dongle *dongle)
{
    if (sim->scheduler_is_edf)
        heap_destroy((t_heap *)dongle->wait_queue);
    else
        fifo_queue_destroy((t_fifo_queue *)dongle->wait_queue);
}

void cleanup_simulation(t_sim *sim)
{
    int i;
    int nb_dongles;

    nb_dongles = sim->nb_coders;
    if (sim->nb_coders == 1)
        nb_dongles = 1;

    i = 0;
    while (i < nb_dongles)
    {
        destroy_wait_queue(sim, &sim->dongles[i]);
        pthread_mutex_destroy(&sim->dongles[i].lock);
        pthread_cond_destroy(&sim->dongles[i].cond);
        i++;
    }
    free(sim->dongles);
    free(sim->coders);
    pthread_mutex_destroy(&sim->log_lock);
    pthread_mutex_destroy(&sim->end_lock);
}