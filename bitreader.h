#ifndef BITREADER_H
#define BITREADER_H

#include <stdint.h>
#include <stdlib.h>
#include "pqueue.h"

struct BitReader {
    uint8_t* buffer;
    size_t size;
    uint8_t padding;
    size_t byte_pos;
    uint8_t bit_pos;
};

struct BitReader* bitreader_init(uint8_t* buffer, size_t size, uint8_t padding);
int bitreader_read_bit(struct BitReader* br);
struct Node* deserialize_tree(struct BitReader* br);
void free_tree(struct Node* root);

#endif
