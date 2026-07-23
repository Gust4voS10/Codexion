#include "codexion.h"

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

void fifo_queue_destroy(t_fifo_queue *queue)
{
    if (!queue)
        return ;
    free(queue->ids);
    free(queue);
}

void fifo_queue_push(t_fifo_queue *queue, int coder_id)
{
    int back;

    back = (queue->front + queue->count) % queue->capacity;
    queue->ids[back] = coder_id;
    queue->count++;
}

int fifo_queue_pop(t_fifo_queue *queue)
{
    int id;

    id = queue->ids[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->count--;
    return (id);
}

int fifo_queue_peek(t_fifo_queue *queue)
{
    if (queue->count == 0)
        return (-1);
    return (queue->ids[queue->front]);
}