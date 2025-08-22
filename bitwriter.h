#ifndef BIT_WRITER_H
#define BIT_WRITER_H

#include <stdint.h>
#include <stdio.h>
#include "pqueue.h"

struct BitWriter {
    FILE* file;
    uint8_t* buffer;// ________//________//________
    size_t capacity;
    uint8_t bit_pos;
    size_t byte_pos;

};

struct HuffmanCode {
    uint32_t code;
    uint8_t length;
};//закодований символ, де code — сам сивол в кодуванні Гаффмана, а length — його кодова довжина

void bitwriter_init(struct BitWriter* bw, FILE* file);
void bitwriter_write_bit(struct BitWriter *bw, int bit);
void bitwriter_write_byte(struct BitWriter *bw, uint8_t byte);
void serialize_tree(struct BitWriter* bw, struct Node* node);
void compress_text(FILE* fin, struct BitWriter* bw, struct HuffmanCode codes[]);

#endif