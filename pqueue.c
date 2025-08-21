#include <stdlib.h>
#include "pqueue.h"

/*****************
    i = index;
    i_left_child = 2i + 1;
    i_right_child = 2i + 2;
    i_parent = (i - 1) / 2.
******************/

static void swap(void** a, void** b) {//універсальний обмін елементами
    void* temp = *a;
    *a = *b;
    *b = temp;
}

static void heapify_up(struct PQueue* queue, unsigned index, int (*cmp)(void*, void*)) {//відновлення інваріанти купи після додавання елемента в чергу
    if (index == 0) return;
    unsigned parentIndex = (index - 1) / 2;
    if (cmp(queue->arr[index], queue->arr[parentIndex]) < 0) {
        swap(&queue->arr[index], &queue->arr[parentIndex]);
        heapify_up(queue, parentIndex, cmp);
    }
}

static void heapify_down(struct PQueue* queue, unsigned index, int (*cmp)(void*, void*)) {//відновлення інваріанти купи після вилучення елемента з черги
    unsigned leftChildIndex = 2 * index + 1;
    unsigned rightChildIndex = 2 * index + 2;
    unsigned smallest = index;

    if (leftChildIndex <= queue->size && cmp(queue->arr[leftChildIndex], queue->arr[smallest]) < 0)
        smallest = leftChildIndex;

    if (rightChildIndex <= queue->size && cmp(queue->arr[rightChildIndex], queue->arr[smallest]) < 0)
        smallest = rightChildIndex;

    if (smallest != index) { //smallest == index — база рекурсії
        swap(&queue->arr[index], &queue->arr[smallest]);
        heapify_down(queue, smallest, cmp);
    }
}

struct PQueue* pqueue_create(unsigned capacity) {
    struct PQueue* queue = (struct PQueue*)malloc(sizeof(struct PQueue));
    if (queue == NULL) return NULL;
    queue->arr = calloc(capacity, sizeof(void*));
    if (queue->arr == NULL) {
        free(queue);
        return NULL;
    }
    queue->capacity = capacity;
    queue->size = 0;
    return queue;
}

void pqueue_free(struct PQueue* queue) {
    if (queue == NULL) return;
    for(int i = 0; i < queue->size; i++) free(queue->arr[i]);//тимчасова річ
    free(queue->arr);
    free(queue);
}

void pqueue_insert(struct PQueue* queue, void* node, int (*cmp)(void*, void*)) {
    if (queue->size == queue->capacity) {
        unsigned new_capacity = queue->capacity * 2;
        void** new_arr = realloc(queue->arr, sizeof(void*) * new_capacity);
        if (new_arr == NULL) return;
        queue->arr = new_arr;
        queue->capacity = new_capacity;
    }
    queue->arr[queue->size] = node;
    heapify_up(queue, queue->size, cmp);
    queue->size++;
}

void* pqueue_poll(struct PQueue* queue, int (*cmp)(void*, void*)) {
    if (queue == NULL || queue->size == 0) return NULL;
    void* min_node = queue->arr[0];
    queue->size--;
    queue->arr[0] = queue->arr[queue->size];
    heapify_down(queue, 0, cmp);
    return min_node;
}