#include "codexion.h"

// Creates a FIFO queue with the given capacity.
t_fifo_queue *fifo_queue_create(int capacity)
{
    t_fifo_queue *queue;

    queue = malloc(sizeof(t_fifo_queue));
    if (!queue)
        return (NULL);
    queue->ids = malloc(sizeof(int) * capacity);
    if (!queue->ids)
    {
        free(queue);
        return (NULL);
    }
    queue->capacity = capacity;
    queue->front = 0;
    queue->count = 0;
    return (queue);
}

// Destroys the FIFO queue and frees associated memory.
void fifo_queue_destroy(t_fifo_queue *queue)
{
    if (!queue)
        return ;
    free(queue->ids);
    free(queue);
}

// Adds a coder ID to the back of the FIFO queue.
void fifo_queue_push(t_fifo_queue *queue, int coder_id)
{
    int back;

    back = (queue->front + queue->count) % queue->capacity;
    queue->ids[back] = coder_id;
    queue->count++;
}

// Removes and returns the front coder ID from the FIFO queue.
int fifo_queue_pop(t_fifo_queue *queue)
{
    int id;

    id = queue->ids[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->count--;
    return (id);
}

// Returns the front coder ID without removing it, or -1 if empty.
int fifo_queue_peek(t_fifo_queue *queue)
{
    if (queue->count == 0)
        return (-1);
    return (queue->ids[queue->front]);
}