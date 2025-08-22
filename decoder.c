#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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

    // 1. Зчитуємо "магічне" число (наприклад, 'HUFF')
    char magic[4];
    fread(magic, sizeof(char), 4, finput);

    // 2. Зчитуємо дерево
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

    // 3. Зчитуємо padding та розмір стиснених даних
    uint8_t dataPadding;
    fread(&dataPadding, sizeof(uint8_t), 1, finput);

    uint8_t dataSize;
    fread(&dataSize, sizeof(uint8_t), 1, finput);

    uint8_t* dataBuffer = malloc(dataSize);
    fread(dataBuffer, sizeof(uint8_t), dataSize, finput);

    struct BitReader* data_br = bitreader_init(dataBuffer, dataSize, dataPadding);

    // 4. Декодуємо
    struct Node* current = root;
    int bit;
    while ((bit = bitreader_read_bit(data_br)) != -1) {
        current = (bit == 0) ? current->left : current->right;

        if (!current->left && !current->right) {
            fputc(current->character, fout);
            current = root;
        }
    }

    // 5. Звільняємо ресурси
    fclose(finput);
    fclose(fout);
    free(data_br);
    free(dataBuffer);
    free_tree(root); // Не забудь реалізувати
    return 0;
}
