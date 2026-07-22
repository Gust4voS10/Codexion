#include "codexion.h"


static void precise_sleep(t_sim *sim, long duration_ms)
{
    long    start;

    start = get_timestamp_ms();
    while (get_timestamp_ms() - start < duration_ms)
    {
        pthread_mutex_lock(&sim->end_lock);
        if (sim->simulation_over)
        {
            pthread_mutex_unlock(&sim->end_lock);
            return ;
        }
        pthread_mutex_unlock(&sim->end_lock);
        usleep(500);
    }
}

static int is_simulation_over(t_sim *sim)
{
    int over;

    pthread_mutex_lock(&sim->end_lock);
    over = sim->simulation_over;
    pthread_mutex_unlock(&sim->end_lock);
    return (over);
}

void *coder_routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;
    coder->last_compile_start = get_timestamp_ms();

    while (!is_simulation_over(coder->sim))
    {
        acquire_dongles(coder);

        coder->last_compile_start = get_timestamp_ms();
        log_state(coder->sim, coder->id, "is compiling");
        precise_sleep(coder->sim, coder->sim->time_to_compile);

        release_dongle(coder->left);
        release_dongle(coder->right);
        coder->compiles_done++;

        if (is_simulation_over(coder->sim))
            break ;

        log_state(coder->sim, coder->id, "is debugging");
        precise_sleep(coder->sim, coder->sim->time_to_debug);

        if (is_simulation_over(coder->sim))
            break ;

        log_state(coder->sim, coder->id, "is refactoring");
        precise_sleep(coder->sim, coder->sim->time_to_refactor);
    }
    return (NULL);
}