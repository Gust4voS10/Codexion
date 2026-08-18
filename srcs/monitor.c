#include "codexion.h"

// Marks the simulation as ended in a thread-safe manner.
static void end_simulation(t_sim *sim)
{
    pthread_mutex_lock(&sim->end_lock);
    sim->simulation_over = 1;
    pthread_mutex_unlock(&sim->end_lock);
}

// Returns 1 if the coder has exceeded its burnout deadline.
static int coder_burned_out(t_coder *coder)
{
    long    elapsed;

    elapsed = get_timestamp_ms() - coder->last_compile_start;
    return (elapsed > coder->sim->time_to_burnout);
}

// Returns 1 when all coders have completed the required number of compiles.
static int everyone_compiled_enough(t_sim *sim)
{
    int i;

    i = 0;
    while (i < sim->nb_coders)
    {
        if (sim->coders[i].compiles_done < sim->compiles_required)
            return (0);
        i++;
    }
    return (1);
}

// Monitor thread routine that ends the simulation on burnout or completion.
void *monitor_routine(void *arg)
{
    t_sim   *sim;
    int     i;

    sim = (t_sim *)arg;
    while (1)
    {
        i = 0;
        while (i < sim->nb_coders)
        {
            if (coder_burned_out(&sim->coders[i]))
            {
                log_state(sim, sim->coders[i].id, "burned out");
                end_simulation(sim);
                return (NULL);
            }
            i++;
        }
        if (everyone_compiled_enough(sim))
        {
            end_simulation(sim);
            return (NULL);
        }
        usleep(1000);
    }
}