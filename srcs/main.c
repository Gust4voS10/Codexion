#include "codexion.h"

// Creates one thread per coder and a monitor thread.
// Returns 1 on success, or 0 if any thread creation fails.
static int create_all_threads(t_sim *sim, pthread_t *monitor)
{
    int i;

    i = 0;
    while (i < sim->nb_coders)
    {
        if (pthread_create(&sim->coders[i].thread, NULL,
                coder_routine, &sim->coders[i]) != 0)
            return (0);
        i++;
    }
    if (pthread_create(monitor, NULL, monitor_routine, sim) != 0)
        return (0);
    return (1);
}

// Waits for the monitor thread and all coder threads to finish.
// Waits for the monitor thread and all coder threads to finish.
static void join_all_threads(t_sim *sim, pthread_t monitor)
{
    int i;

    pthread_join(monitor, NULL);
    i = 0;
    while (i < sim->nb_coders)
    {
        pthread_join(sim->coders[i].thread, NULL);
        i++;
    }
}

// Entry point for the simulation program.
int main(int argc, char **argv)
{
    t_sim       sim;
    pthread_t   monitor;

    if (argc != 9)
    {
        fprintf(stderr, "Error: wrong number of arguments\n");
        return (1);
    }
    if (!parse_args(argc, argv))
    {
        fprintf(stderr, "Error: invalid arguments\n");
        return (1);
    }
    if (!init_simulation(&sim, argv))
    {
        fprintf(stderr, "Error: initialization failed\n");
        return (1);
    }
    sim.start_time = get_timestamp_ms();
    if (!create_all_threads(&sim, &monitor))
    {
        fprintf(stderr, "Error: thread creation failed\n");
        return (1);
    }
    join_all_threads(&sim, monitor);
    cleanup_simulation(&sim);
    return (0);
}