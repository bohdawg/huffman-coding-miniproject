#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHAR_TABLE_SIZE 256

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
    for (int i = 0; i < CHAR_TABLE_SIZE; i++) {
        if (frequencies[i] > 0) {
            if ((unsigned char)i == '\n') printf("'\\n': %u\n", frequencies[i]);
            else printf("'%c': %u\n", (unsigned char)i, frequencies[i]);
        }
    }
    fclose(finput);
    return 0;
}