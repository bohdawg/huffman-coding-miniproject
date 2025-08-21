#ifndef PQUEUE_H
#define PQUEUE_H

struct PQueue {
    void** arr;//масив невідомого типу (для універсальності)
    unsigned size;//поточний розмір
    unsigned capacity;//поточна максимальна межа
};

struct PQueue* pqueue_create(unsigned capacity);//створення черги з пріоритетом
void pqueue_free(struct PQueue* queue);//звільнення пам'яті черги
void pqueue_insert(struct PQueue* queue, void* node, int (*cmp)(void*, void*));//вставлення вузла в чергу
void* pqueue_poll(struct PQueue* queue, int (*cmp)(void*, void*));//вилучення елемента з черги

#endif