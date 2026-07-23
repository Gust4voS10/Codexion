#ifndef CODEXION_H
# define CODEXION_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

typedef struct s_dongle
{
    int             id;
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    int             is_free;
    long            released_at_ms;
    void            *wait_queue;
}   t_dongle;

typedef struct s_coder
{
    int             id;
    long            last_compile_start;
    int             compiles_done;
    t_dongle        *left;
    t_dongle        *right;
    pthread_t       thread;
    struct s_sim    *sim;
}   t_coder;

typedef struct s_sim
{
    int             nb_coders;
    long            time_to_burnout;
    long            time_to_compile;
    long            time_to_debug;
    long            time_to_refactor;
    int             compiles_required;
    long            dongle_cooldown;
    int             scheduler_is_edf;

    t_coder         *coders;
    t_dongle        *dongles;

    pthread_mutex_t log_lock;
    pthread_mutex_t end_lock;
    int             simulation_over;
    long            start_time;
}   t_sim;

typedef struct s_dongle
{
    int             id;
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    int             is_free;
    long            released_at_ms;
}   t_dongle;

typedef struct s_fifo_queue
{
    int     *ids;
    int     capacity;
    int     front;
    int     count;
}   t_fifo_queue;

typedef struct s_heap
{
    t_coder **data;
    int     size;
    int     capacity;
}   t_heap;


#endif