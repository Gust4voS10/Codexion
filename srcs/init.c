#include "codexion.h"

// Initializes simulation parameters from command-line arguments.
void init_sim_params(t_sim *sim, char **argv)
{
    sim->nb_coders = atoi(argv[1]);
    sim->time_to_burnout = atol(argv[2]);
    sim->time_to_compile = atol(argv[3]);
    sim->time_to_debug = atol(argv[4]);
    sim->time_to_refactor = atol(argv[5]);
    sim->compiles_required = atoi(argv[6]);
    sim->dongle_cooldown = atol(argv[7]);
    sim->scheduler_is_edf = (strcmp(argv[8], "edf") == 0);
    sim->simulation_over = 0;
}

// Initializes a single dongle, including its lock, condition, and wait queue.
static int init_one_dongle(t_sim *sim, t_dongle *dongle, int id)
{
    dongle->id = id;
    dongle->is_free = 1;
    dongle->released_at_ms = 0;
    if (pthread_mutex_init(&dongle->lock, NULL) != 0)
        return (0);
    if (pthread_cond_init(&dongle->cond, NULL) != 0)
        return (0);
    if (sim->scheduler_is_edf)
        dongle->wait_queue = heap_create(sim->nb_coders);
    else
        dongle->wait_queue = fifo_queue_create(sim->nb_coders);
    if (!dongle->wait_queue)
        return (0);
    return (1);
}

// Allocates and initializes all dongles used by the simulation.
static int init_dongles(t_sim *sim)
{
    int i;
    int nb_dongles;

    nb_dongles = sim->nb_coders;
    if (sim->nb_coders == 1)
        nb_dongles = 1;
    sim->dongles = malloc(sizeof(t_dongle) * nb_dongles);
    if (!sim->dongles)
        return (0);
    i = 0;
    while (i < nb_dongles)
    {
        if (!init_one_dongle(sim, &sim->dongles[i], i))
            return (0);
        i++;
    }
    return (1);
}

// Allocates and initializes all coder structures and assigns dongles.
static int init_coders(t_sim *sim)
{
    int i;

    sim->coders = malloc(sizeof(t_coder) * sim->nb_coders);
    if (!sim->coders)
        return (0);
    i = 0;
    while (i < sim->nb_coders)
    {
        sim->coders[i].id = i + 1;
        sim->coders[i].compiles_done = 0;
        sim->coders[i].last_compile_start = 0;
        sim->coders[i].sim = sim;
        if (sim->nb_coders == 1)
        {
            sim->coders[i].left = &sim->dongles[0];
            sim->coders[i].right = &sim->dongles[0];
        }
        else
        {
            sim->coders[i].left = &sim->dongles[i];
            sim->coders[i].right = &sim->dongles[(i + 1) % sim->nb_coders];
        }
        i++;
    }
    return (1);
}

// Initializes the simulation environment, including mutexes, dongles, and coders.
int init_simulation(t_sim *sim, char **argv)
{
    init_sim_params(sim, argv);
    if (pthread_mutex_init(&sim->log_lock, NULL) != 0)
        return (0);
    if (pthread_mutex_init(&sim->end_lock, NULL) != 0)
        return (0);
    if (!init_dongles(sim))
        return (0);
    if (!init_coders(sim))
        return (0);
    return (1);
}