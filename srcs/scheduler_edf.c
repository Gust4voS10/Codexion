#include "codexion.h"

t_heap *heap_create(int capacity)
{
    t_heap  *heap;

    heap = malloc(sizeof(t_heap));
    if (!heap)
        return (NULL);
    heap->data = malloc(sizeof(t_coder *) * capacity);
    if (!heap->data)
    {
        free(heap);
        return (NULL);
    }
    heap->size = 0;
    heap->capacity = capacity;
    return (heap);
}

void heap_destroy(t_heap *heap)
{
    if (!heap)
        return ;
    free(heap->data);
    free(heap);
}

static long deadline_of(t_coder *coder)
{
    return (coder->last_compile_start + coder->sim->time_to_burnout);
}

static int has_priority(t_coder *a, t_coder *b)
{
    if (deadline_of(a) != deadline_of(b))
        return (deadline_of(a) < deadline_of(b));
    return (a->id < b->id);
}

static void swap(t_coder **a, t_coder **b)
{
    t_coder *tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}

void heap_push(t_heap *heap, t_coder *coder)
{
    int i;
    int parent;

    heap->data[heap->size] = coder;
    i = heap->size;
    heap->size++;
    while (i > 0)
    {
        parent = (i - 1) / 2;
        if (!has_priority(heap->data[i], heap->data[parent]))
            break ;
        swap(&heap->data[i], &heap->data[parent]);
        i = parent;
    }
}

t_coder *heap_pop(t_heap *heap)
{
    t_coder *top;
    int     i;
    int     left;
    int     right;
    int     smallest;

    top = heap->data[0];
    heap->size--;
    heap->data[0] = heap->data[heap->size];

    i = 0;
    while (1)
    {
        left = i * 2 + 1;
        right = i * 2 + 2;
        smallest = i;
        if (left < heap->size && has_priority(heap->data[left], heap->data[smallest]))
            smallest = left;
        if (right < heap->size && has_priority(heap->data[right], heap->data[smallest]))
            smallest = right;
        if (smallest == i)
            break ;
        swap(&heap->data[i], &heap->data[smallest]);
        i = smallest;
    }
    return (top);
}

t_coder *heap_peek(t_heap *heap)
{
    if (heap->size == 0)
        return (NULL);
    return (heap->data[0]);
}