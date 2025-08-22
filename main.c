#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "pqueue.h"
#include "bitwriter.h"
#define CHAR_TABLE_SIZE 256
#define HUFFMAN_FILE_SIGNATURE "HUFF"

int frequencies_comparator(void* a, void* b) {//порівняння вузлів дерева Гаффмана за частотами (для пріоритетної черги)
    struct Node* node1 = (struct Node*)a;
    struct Node* node2 = (struct Node*)b;
    if (node1->frequency > node2->frequency) return 1;
    if (node1->frequency < node2->frequency) return -1;
    return 0;
}

void traverseHuffmanTree(struct Node* root, struct HuffmanCode codes[], uint32_t code, uint8_t length) {
    if (root == NULL) return;
    if (root->left == NULL && root->right == NULL) {
        codes[root->character].code = code;
        codes[root->character].length = length;
    }
    traverseHuffmanTree(root->left, codes, (code << 1), length + 1);
    traverseHuffmanTree(root->right, codes, (code << 1) | 1, length + 1);//ого! Дякую ШІ
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s [text file to convert]", argv[0]);
        return 1;
    }
    //відкриття вхідного файлу
    FILE* finput = NULL;
    FILE* fout = NULL;
    if ((finput = fopen(argv[1], "rt")) == NULL) {
        perror("Cannot open the input file");
        return 1;
    }
    unsigned int frequencies[CHAR_TABLE_SIZE] = {0};//ініціалізація масиву частот
    int c = 0;//ініціалізація символьного буферу
    while ((c = fgetc(finput)) != EOF) frequencies[(unsigned char)c]++;//посимвольне читтання файлу й збільшення індексів в масиві частот, чиє числове значення відповідає номеру символу в таблиці ASCII; додаткове виразне приведення типу
    fclose(finput);
    struct PQueue* queue = pqueue_create(CHAR_TABLE_SIZE >> 4);//створення черги з пріоритетом з початковою ємністю CHAR_TABLE_SIZE / 2^4
    for (int i = 0; i < CHAR_TABLE_SIZE; i++) {
        if (frequencies[i] > 0) {
            struct Node* newNode = (struct Node*)calloc(1, sizeof(struct Node));
            newNode->character = (unsigned char)i;
            newNode->frequency = frequencies[(unsigned char)i];
            pqueue_insert(queue, newNode, frequencies_comparator);
        }
    }
    while (queue->size > 1) {
        struct Node* left = pqueue_poll(queue, frequencies_comparator);
        struct Node* right = pqueue_poll(queue, frequencies_comparator);
        struct Node* internalNode = (struct Node*)calloc(1, sizeof(struct Node));
        internalNode->frequency = left->frequency + right->frequency;
        internalNode->left = left;
        internalNode->right = right;
        pqueue_insert(queue, internalNode, frequencies_comparator);
    }
    struct Node* HuffmanTreeRoot = queue->arr[0];//корінь дерева Гаффмана
    struct HuffmanCode codes[CHAR_TABLE_SIZE] = {0};//таблиця кодів, де символ — індекс.
    traverseHuffmanTree(HuffmanTreeRoot, codes, 0, 0);
    fout = fopen("output.huff", "wb");
    fwrite(HUFFMAN_FILE_SIGNATURE, sizeof(char), sizeof(HUFFMAN_FILE_SIGNATURE) / sizeof(char), fout);
    struct BitWriter bw_tree, bw_data;
    bitwriter_init(&bw_tree, fout);
    bitwriter_init(&bw_data, fout);
    serialize_tree(&bw_tree, HuffmanTreeRoot);
    uint8_t treePadding = (8 - bw_tree.bit_pos) % 8;
    uint32_t treeSize = bw_tree.byte_pos + 1;
    fwrite(&treeSize, sizeof(uint32_t), 1, bw_tree.file);
    fwrite(&treePadding, sizeof(uint8_t), 1, bw_tree.file);
    fwrite(bw_tree.buffer, sizeof(uint8_t), treeSize, bw_tree.file);
    free(bw_tree.buffer);
    if ((finput = fopen(argv[1], "rt")) == NULL) {
        perror("Cannot open the input file");
        return 1;
    }
    compress_text(finput, &bw_data, codes);
    uint8_t dataPadding = (8 - bw_data.bit_pos) % 8;
    uint32_t dataSize = bw_data.byte_pos + 1;
    fwrite(&dataPadding, sizeof(uint8_t), 1, fout);
    fwrite(bw_data.buffer, sizeof(uint8_t), dataSize, fout);
    free(bw_data.buffer);
    fclose(finput);
    pqueue_free(queue);
    return 0;
}