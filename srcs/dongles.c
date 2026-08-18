#include "codexion.h"

// Returns 1 if the coder successfully acquired both dongles, or 0 if it
// gave up because the simulation ended (for example, another coder burned
// out) while waiting. If it acquired only the first dongle and the
// simulation ended before the second, it releases the first before
// returning.
int acquire_dongles(t_coder *coder)
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

    if (!request_dongle(coder, first))
        return (0);
    log_state(coder->sim, coder->id, "has taken a dongle");

    if (!request_dongle(coder, second))
    {
        release_dongle(first);
        return (0);
    }
    log_state(coder->sim, coder->id, "has taken a dongle");
    return (1);
}

// Builds an absolute timeout timestamp representing "ms_from_now"
// milliseconds from now, in the format required by pthread_cond_timedwait
// (struct timespec).
// Builds an absolute timeout timestamp representing "ms_from_now"
// milliseconds from now, in the format required by pthread_cond_timedwait
// (struct timespec).
static void build_timeout(struct timespec *ts, long ms_from_now)
{
    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_sec += ms_from_now / 1000;
    ts->tv_nsec += (ms_from_now % 1000) * 1000000L;
    if (ts->tv_nsec >= 1000000000L)
    {
        ts->tv_sec += 1;
        ts->tv_nsec -= 1000000000L;
    }
}

// Attempts to acquire a dongle for the coder, waiting with a timeout
// to handle cooldown expiration and simulation termination.
int request_dongle(t_coder *coder, t_dongle *dongle)
{
    struct timespec ts;

    pthread_mutex_lock(&dongle->lock);

    sched_enqueue(coder, dongle);

    // We use cond_timedwait instead of cond_wait because the cooldown
    // expires "on its own" without any signal/broadcast at that moment.
    // If we used plain cond_wait, the thread could sleep forever waiting
    // for a signal that would never arrive, even though the dongle is
    // already available. Waking every few milliseconds ensures the
    // condition (including cooldown) is always rechecked.
    //
    // We also check sim_is_over: without this, if the simulation ends
    // (for example, another coder burned out) while this coder is waiting
    // for a dongle that will never appear, it would remain stuck here
    // forever and the program would never complete (pthread_join hung).
    while (!(dongle_is_available(dongle, coder->sim->dongle_cooldown)
            && sched_is_my_turn(coder, dongle)))
    {
        if (sim_is_over(coder->sim))
        {
            pthread_mutex_unlock(&dongle->lock);
            return (0);
        }
        build_timeout(&ts, 2);
        pthread_cond_timedwait(&dongle->cond, &dongle->lock, &ts);
    }

    sched_remove_top(coder, dongle);
    dongle->is_free = 0;

    pthread_mutex_unlock(&dongle->lock);
    return (1);
}

// Returns 1 if the dongle is free and its cooldown period has expired.
// Returns 1 if the dongle is free and its cooldown period has expired.
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

// Releases the dongle, updates its timestamp, and notifies waiting coders.
// Releases the dongle, updates its release timestamp, and notifies waiters.
void release_dongle(t_dongle *dongle)
{
    pthread_mutex_lock(&dongle->lock);
    dongle->is_free = 1;
    dongle->released_at_ms = get_timestamp_ms();
    pthread_cond_broadcast(&dongle->cond);
    pthread_mutex_unlock(&dongle->lock);
}

