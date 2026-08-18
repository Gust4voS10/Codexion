#include "codexion.h"


// Sleeps for a precise duration while checking for simulation end.
static void precise_sleep(t_sim *sim, long duration_ms)
{
    long    start;

    start = get_timestamp_ms();
    while (get_timestamp_ms() - start < duration_ms)
    {
        if (sim_is_over(sim))
            return ;
        usleep(500);
    }
}

// Main routine for each coder thread, executing compile/debug/refactor cycles.
void *coder_routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;
    coder->last_compile_start = get_timestamp_ms();

    while (!sim_is_over(coder->sim))
    {
        if (!acquire_dongles(coder))
            break ;

        coder->last_compile_start = get_timestamp_ms();
        log_state(coder->sim, coder->id, "is compiling");
        precise_sleep(coder->sim, coder->sim->time_to_compile);

        release_dongle(coder->left);
        release_dongle(coder->right);
        coder->compiles_done++;

        if (sim_is_over(coder->sim))
            break ;

        log_state(coder->sim, coder->id, "is debugging");
        precise_sleep(coder->sim, coder->sim->time_to_debug);

        if (sim_is_over(coder->sim))
            break ;

        log_state(coder->sim, coder->id, "is refactoring");
        precise_sleep(coder->sim, coder->sim->time_to_refactor);
    }
    return (NULL);
}