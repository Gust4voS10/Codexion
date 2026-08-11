#ifndef CODEXION_H
# define CODEXION_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

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

/* parsing.c */
int             is_valid_number(char *str);
int             parse_args(int argc, char **argv);

/* utils.c */
long            get_timestamp_ms(void);
int             sim_is_over(t_sim *sim);

/* logger.c */
void            log_state(t_sim *sim, int coder_id, const char *message);

/* dongles.c */
int             acquire_dongles(t_coder *coder);
int             request_dongle(t_coder *coder, t_dongle *dongle);
int             dongle_is_available(t_dongle *dongle, long cooldown);
void            release_dongle(t_dongle *dongle);

/* coder.c */
void            *coder_routine(void *arg);

/* monitor.c */
void            *monitor_routine(void *arg);

/* scheduler.c */
void            sched_enqueue(t_coder *coder, t_dongle *dongle);
int             sched_is_my_turn(t_coder *coder, t_dongle *dongle);
void            sched_remove_top(t_coder *coder, t_dongle *dongle);

/* scheduler_fifo.c */
t_fifo_queue    *fifo_queue_create(int capacity);
void            fifo_queue_destroy(t_fifo_queue *queue);
void            fifo_queue_push(t_fifo_queue *queue, int coder_id);
int             fifo_queue_pop(t_fifo_queue *queue);
int             fifo_queue_peek(t_fifo_queue *queue);

/* scheduler_edf.c */
t_heap          *heap_create(int capacity);
void            heap_destroy(t_heap *heap);
void            heap_push(t_heap *heap, t_coder *coder);
t_coder         *heap_pop(t_heap *heap);
t_coder         *heap_peek(t_heap *heap);

/* init.c */
void            init_sim_params(t_sim *sim, char **argv);
int             init_simulation(t_sim *sim, char **argv);

/* cleanup.c */
void            cleanup_simulation(t_sim *sim);

#endif