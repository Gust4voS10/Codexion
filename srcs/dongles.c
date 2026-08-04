#include "codexion.h"

void acquire_dongles(t_coder *coder)
{
    t_dongle    *first;
    t_dongle    *second;

    if (coder->id == coder->sim->nb_coders)
    {
        first = coder->right;
        second = coder->left;
    }
    else
    {
        first = coder->left;
        second = coder->right;
    }

    request_dongle(coder, first);
    log_state(coder->sim, coder->id, "has taken a dongle");

    request_dongle(coder, second);
    log_state(coder->sim, coder->id, "has taken a dongle");
}

void request_dongle(t_coder *coder, t_dongle *dongle)
{
    pthread_mutex_lock(&dongle->lock);

    sched_enqueue(coder, dongle);

    while (!(dongle_is_available(dongle, coder->sim->dongle_cooldown)
            && sched_is_my_turn(coder, dongle)))
        pthread_cond_wait(&dongle->cond, &dongle->lock);

    sched_remove_top(coder, dongle);
    dongle->is_free = 0;

    pthread_mutex_unlock(&dongle->lock);
}

int dongle_is_available(t_dongle *dongle, long cooldown)
{
    long    now;

    if (!dongle->is_free)
        return (0);
    now = get_timestamp_ms();
    if (now - dongle->released_at_ms < cooldown)
        return (0);
    return (1);
}

void release_dongle(t_dongle *dongle)
{
    pthread_mutex_lock(&dongle->lock);
    dongle->is_free = 1;
    dongle->released_at_ms = get_timestamp_ms();
    pthread_cond_broadcast(&dongle->cond);
    pthread_mutex_unlock(&dongle->lock);
}

