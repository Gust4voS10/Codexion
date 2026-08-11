#include "codexion.h"

// Retorna 1 se conseguiu pegar os dois dongles, ou 0 se desistiu
// porque a simulação terminou (ex: outro coder queimou) enquanto
// esperava. Se conseguiu só o primeiro dongle e a simulação acabou
// antes do segundo, devolve o primeiro antes de retornar.
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

// Monta um timestamp absoluto "daqui a ms_from_now milissegundos",
// no formato que pthread_cond_timedwait exige (struct timespec).
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

int request_dongle(t_coder *coder, t_dongle *dongle)
{
    struct timespec ts;

    pthread_mutex_lock(&dongle->lock);

    sched_enqueue(coder, dongle);

    // Usamos cond_timedwait (em vez de cond_wait) porque o cooldown
    // expira "sozinho", sem que ninguém chame signal/broadcast nesse
    // momento. Se usássemos cond_wait puro, a thread poderia dormir
    // para sempre esperando um sinal que nunca chegaria, mesmo com
    // o dongle já disponível. Acordar a cada poucos ms garante que a
    // condição (incluindo o cooldown) seja sempre reavaliada.
    //
    // Também checamos sim_is_over: sem isso, se a simulação terminar
    // (ex: outro coder queimou) enquanto este coder espera um dongle
    // que nunca vai chegar, ele ficaria preso aqui para sempre, e o
    // programa nunca conseguiria terminar (pthread_join travado).
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

