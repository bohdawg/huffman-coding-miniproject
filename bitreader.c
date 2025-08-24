#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "bitreader.h"
#include "pqueue.h" // для struct Node

// Ініціалізація BitReader
struct BitReader* bitreader_init(uint8_t* buffer, size_t size, uint8_t padding) {
    struct BitReader* br = malloc(sizeof(struct BitReader));
    if (!br) {
        perror("Cannot allocate BitReader");
        return NULL;
    }

    br->buffer = buffer;
    br->size = size;
    br->padding = padding;
    br->byte_pos = 0;
    br->bit_pos = 0;

    return br;
}

// Зчитування біта
int bitreader_read_bit(struct BitReader* br) {
    if (br->byte_pos >= br->size) return -1;

    // Перевірка останнього байта з паддінгом
    if (br->byte_pos == br->size - 1 && br->bit_pos >= (8 - br->padding)) {
        return -1;
    }

    uint8_t byte = br->buffer[br->byte_pos];
    int bit = (byte >> (7 - br->bit_pos)) & 1;

    br->bit_pos++;
    if (br->bit_pos == 8) {
        br->bit_pos = 0;
        br->byte_pos++;
    }

    return bit;
}

// Рекурсивна десеріалізація дерева
struct Node* deserialize_tree(struct BitReader* br) {
    int bit = bitreader_read_bit(br);
    if (bit == -1) return NULL;

    if (bit == 1) {
        // Зчитуємо символ
        uint8_t byte = 0;
        for (int i = 7; i >= 0; i--) {
            int b = bitreader_read_bit(br);
            if (b == -1) return NULL;
            byte |= (b << i);
        }

        struct Node* leaf = malloc(sizeof(struct Node));
        if (!leaf) {
            perror("Cannot allocate leaf node");
            return NULL;
        }
        leaf->character = byte;
        leaf->left = NULL;
        leaf->right = NULL;
        return leaf;
    } else {
        // Внутрішній вузол
        struct Node* node = malloc(sizeof(struct Node));
        if (!node) {
            perror("Cannot allocate internal node");
            return NULL;
        }
        node->left = deserialize_tree(br);
        node->right = deserialize_tree(br);
        return node;
    }
}

void free_tree(struct Node* root) {
    if (root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}