#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pqueue.h"
#define CHAR_TABLE_SIZE 256

struct Node {
    unsigned char character;
    unsigned int frequency;
    struct Node* left;
    struct Node* right;
};//вузол мінімальної купи та дерева Гаффмана

int comparator(struct Node* node1, struct Node* node2) {//порівняння вузлів дерева Гаффмана за частотами (для пріоритетної черги)
    if (node1->frequency > node2->frequency) return 1;
    if (node1->frequency < node2->frequency) return -1;
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s [text file to convert]", argv[0]);
        return 1;
    }
    //відкриття вхідного файлу
    FILE* finput = NULL;
    if ((finput = fopen(argv[1], "rt")) == NULL) {
        perror("Cannot open the input file");
        return 1;
    }
    unsigned int frequencies[CHAR_TABLE_SIZE] = {0};//ініціалізація масиву частот
    int c = 0;//ініціалізація символьного буферу
    while ((c = fgetc(finput)) != EOF) frequencies[(unsigned char)c]++;//посимвольне читтання файлу й збільшення індексів в масиві частот, чиє числове значення відповідає номеру символу в таблиці ASCII; додаткове виразне приведення типу
    struct PQueue* queque = pqueue_create(CHAR_TABLE_SIZE >> 4);//створення черги з пріоритетом з початковою ємністю CHAR_TABLE_SIZE / 2^4
    for (int i = 0; i < CHAR_TABLE_SIZE; i++) {
        if (frequencies[i] > 0) {
            struct Node* newNode = (struct Node*)calloc(1, sizeof(struct Node));
            newNode->character = (unsigned char)i;
            newNode->frequency = frequencies[(unsigned char)i];
            pqueue_insert(queque, newNode, comparator);
        }
    }
    fclose(finput);
    return 0;
}