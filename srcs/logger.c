#include "codexion.h"

void log_state(t_sim *sim, int coder_id, const char *message)
{
    long    timestamp;

    pthread_mutex_lock(&sim->log_lock);

    pthread_mutex_lock(&sim->end_lock);
    if (sim->simulation_over && strcmp(message, "burned out") != 0)
    {
        pthread_mutex_unlock(&sim->end_lock);
        pthread_mutex_unlock(&sim->log_lock);
        return ;
    }
    pthread_mutex_unlock(&sim->end_lock);

    timestamp = get_timestamp_ms() - sim->start_time;
    printf("%ld %d %s\n", timestamp, coder_id, message);

    pthread_mutex_unlock(&sim->log_lock);
}