#include "bitwriter.h"
#include "pqueue.h"
#include <stdlib.h>
#include <string.h>

void bitwriter_init(struct BitWriter* bw, FILE* file) {
    bw->file = file;
    bw->capacity = 4;
    bw->buffer = (uint8_t*)calloc(bw->capacity, sizeof(uint8_t));
    if (bw->buffer == NULL) {
        perror("Cannot initialize bit buffer");
        return;
    }
    bw->bit_pos = 0;
    bw->byte_pos = 0;
}

void bitwriter_write_bit(struct BitWriter* bw, int bit) {
    if (bw->bit_pos == 8) {
        bw->byte_pos++;
        bw->bit_pos = 0;
        if (bw->byte_pos == bw->capacity) {
            size_t new_capacity = bw->capacity << 1;
            uint8_t* new_buffer = realloc(bw->buffer, new_capacity * sizeof(uint8_t));
            if (!new_buffer) {
                perror("Cannot update bit buffer");
                return;
            }
            memset(new_buffer + bw->capacity, 0, (new_capacity - bw->capacity) * sizeof(uint8_t));
            bw->buffer = new_buffer;
            bw->capacity = new_capacity;
        }
    }
    if (bit == 1) bw->buffer[bw->byte_pos] |= (1 << (7 - bw->bit_pos));
    bw->bit_pos++;
}

void bitwriter_write_byte(struct BitWriter* bw, uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        bitwriter_write_bit(bw, (byte >> i) & 1);
    }
}

void serialize_tree(struct BitWriter* bw, struct Node* node) {
    if (!node->left && !node->right) {
        // Листовий вузол: пишемо 1, потім символ (8 біт)
        bitwriter_write_bit(bw, 1);
        bitwriter_write_byte(bw, (uint8_t)node->character);
    } else {
        // Внутрішній вузол: пишемо 0, потім рекурсивно ліве і праве піддерево
        bitwriter_write_bit(bw, 0);
        serialize_tree(bw, node->left);
        serialize_tree(bw, node->right);
    }
}

void compress_text(FILE* fin, struct BitWriter* bw, struct HuffmanCode codes[]) {
    int c = 0;
    while ((c = fgetc(fin)) != EOF) {
        struct HuffmanCode currentCode = codes[(uint8_t)c];
        for (int i = currentCode.length - 1; i >= 0; i--) {
            int bit = (currentCode.code >> i) & 1;
            bitwriter_write_bit(bw, bit);
        }
    }
}