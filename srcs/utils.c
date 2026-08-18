#include "codexion.h"

// Returns the current timestamp in milliseconds since the epoch.
long get_timestamp_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

// Checks safely (with a mutex) whether the simulation has ended.
// Used by both coder.c (main loop) and dongles.c to avoid waiting for a
// dongle forever after the simulation has ended.
int sim_is_over(t_sim *sim)
{
    int over;

    pthread_mutex_lock(&sim->end_lock);
    over = sim->simulation_over;
    pthread_mutex_unlock(&sim->end_lock);
    return (over);
}