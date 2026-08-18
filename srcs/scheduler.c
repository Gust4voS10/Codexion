#include "codexion.h"

// Adds the coder to the dongle wait queue using the selected scheduler.
void sched_enqueue(t_coder *coder, t_dongle *dongle)
{
    if (coder->sim->scheduler_is_edf)
        heap_push((t_heap *)dongle->wait_queue, coder);
    else
        fifo_queue_push((t_fifo_queue *)dongle->wait_queue, coder->id);
}

// Checks whether it is this coder's turn to acquire the dongle.
int sched_is_my_turn(t_coder *coder, t_dongle *dongle)
{
    if (coder->sim->scheduler_is_edf)
        return (heap_peek((t_heap *)dongle->wait_queue) == coder);
    return (fifo_queue_peek((t_fifo_queue *)dongle->wait_queue) == coder->id);
}

// Removes the top coder from the wait queue after allocation.
void sched_remove_top(t_coder *coder, t_dongle *dongle)
{
    if (coder->sim->scheduler_is_edf)
        heap_pop((t_heap *)dongle->wait_queue);
    else
        fifo_queue_pop((t_fifo_queue *)dongle->wait_queue);
    (void)coder;
}