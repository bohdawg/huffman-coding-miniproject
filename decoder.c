#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "pqueue.h"
#include "bitreader.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s [file.huff]\n", argv[0]);
        return 1;
    }

    FILE* finput = fopen(argv[1], "rb");
    if (!finput) {
        perror("Cannot open the input file");
        return 1;
    }

    FILE* fout = fopen("decoded.txt", "wb");
    if (!fout) {
        perror("Cannot create output file");
        fclose(finput);
        return 1;
    }

    char magic[5] = {0};
    fread(magic, sizeof(char), 4, finput);
    if (strncmp(magic, "HUFF", 4) != 0) {
        fprintf(stderr, "Invalid file format (missing HUFF header)\n");
        fclose(finput);
        fclose(fout);
        return 1;
    }

    uint32_t treeSize;
    uint8_t treePadding;
    fread(&treeSize, sizeof(uint32_t), 1, finput);
    fread(&treePadding, sizeof(uint8_t), 1, finput);

    uint8_t* treeBuffer = malloc(treeSize);
    fread(treeBuffer, sizeof(uint8_t), treeSize, finput);

    struct BitReader* tree_br = bitreader_init(treeBuffer, treeSize, treePadding);
    struct Node* root = deserialize_tree(tree_br);
    free(tree_br);
    free(treeBuffer);

    uint32_t dataSize;
    fread(&dataSize, sizeof(uint32_t), 1, finput);

    uint8_t dataPadding;
    fread(&dataPadding, sizeof(uint8_t), 1, finput);

    uint8_t* dataBuffer = malloc(dataSize);
    fread(dataBuffer, sizeof(uint8_t), dataSize, finput);

    struct BitReader* data_br = bitreader_init(dataBuffer, dataSize, dataPadding);

    struct Node* current = root;
    int bit;
    while ((bit = bitreader_read_bit(data_br)) != -1) {
        current = (bit == 0) ? current->left : current->right;

        if (!current->left && !current->right) {
            fputc(current->character, fout);
            current = root;
        }
    }

    fclose(finput);
    fclose(fout);
    free(data_br);
    free(dataBuffer);
    free_tree(root);

    return 0;
}
