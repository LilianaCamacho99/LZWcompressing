#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DICT_SIZE 4096

typedef struct {
    int code;
    char *value;
} DictionaryEntry;

DictionaryEntry *initializeDictionary(int *dictSize) {
    DictionaryEntry *dictionary = (DictionaryEntry *)malloc(MAX_DICT_SIZE * sizeof(DictionaryEntry));
    for (int i = 0; i < 256; i++) {
        dictionary[i].code = i;
        dictionary[i].value = (char *)malloc(2 * sizeof(char));
        dictionary[i].value[0] = i;
        dictionary[i].value[1] = '\0';
    }
    *dictSize = 256;
    return dictionary;
}

int findInDictionary(DictionaryEntry *dictionary, int dictSize, const char *str) {
    for (int i = 0; i < dictSize; i++) {
        if (strcmp(dictionary[i].value, str) == 0) {
            return dictionary[i].code;
        }
    }
    return -1;
}

void compressFile(const char *inputFile, const char *outputFile) {
    FILE *in = fopen(inputFile, "r");
    FILE *out = fopen(outputFile, "wb");

    if (in == NULL || out == NULL) {
        printf("Error abriendo archivos.\n");
        exit(1);
    }

    DictionaryEntry *dictionary;
    int dictSize;
    dictionary = initializeDictionary(&dictSize);

    char current[1024] = {0};
    int currentLength = 0;
    char nextChar;

    while ((nextChar = fgetc(in)) != EOF) {
        current[currentLength++] = nextChar;
        current[currentLength] = '\0';

        if (findInDictionary(dictionary, dictSize, current) == -1) {
            current[--currentLength] = '\0'; // Retroceder

            int code = findInDictionary(dictionary, dictSize, current);
            fwrite(&code, sizeof(int), 1, out);

            current[currentLength++] = nextChar;
            current[currentLength] = '\0';

            dictionary[dictSize].code = dictSize;
            dictionary[dictSize].value = (char *)malloc((strlen(current) + 1) * sizeof(char));
            strcpy(dictionary[dictSize].value, current);
            dictSize++;

            current[0] = nextChar;
            current[1] = '\0';
            currentLength = 1;
        }
    }

    int code = findInDictionary(dictionary, dictSize, current);
    fwrite(&code, sizeof(int), 1, out);

    fclose(in);
    fclose(out);

    printf("Archivo comprimido exitosamente.\n");
}
Descompresión LZW en C
c
Copiar código
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DICT_SIZE 4096

typedef struct {
    int code;
    char *value;
} DictionaryEntry;

DictionaryEntry *initializeDictionary(int *dictSize) {
    DictionaryEntry *dictionary = (DictionaryEntry *)malloc(MAX_DICT_SIZE * sizeof(DictionaryEntry));
    for (int i = 0; i < 256; i++) {
        dictionary[i].code = i;
        dictionary[i].value = (char *)malloc(2 * sizeof(char));
        dictionary[i].value[0] = i;
        dictionary[i].value[1] = '\0';
    }
    *dictSize = 256;
    return dictionary;
}

void decompressFile(const char *inputFile, const char *outputFile) {
    FILE *in = fopen(inputFile, "rb");
    FILE *out = fopen(outputFile, "w");

    if (in == NULL || out == NULL) {
        printf("Error abriendo archivos.\n");
        exit(1);
    }

    DictionaryEntry *dictionary;
    int dictSize;
    dictionary = initializeDictionary(&dictSize);

    int prevCode, currentCode;
    fread(&prevCode, sizeof(int), 1, in);
    fprintf(out, "%s", dictionary[prevCode].value);

    char newEntry[1024];

    while (fread(&currentCode, sizeof(int), 1, in)) {
        if (currentCode < dictSize) {
            fprintf(out, "%s", dictionary[currentCode].value);

            snprintf(newEntry, sizeof(newEntry), "%s%c", dictionary[prevCode].value, dictionary[currentCode].value[0]);
        } else {
            snprintf(newEntry, sizeof(newEntry), "%s%c", dictionary[prevCode].value, dictionary[prevCode].value[0]);
            fprintf(out, "%s", newEntry);
        }

        dictionary[dictSize].code = dictSize;
        dictionary[dictSize].value = (char *)malloc((strlen(newEntry) + 1) * sizeof(char));
        strcpy(dictionary[dictSize].value, newEntry);
        dictSize++;

        prevCode = currentCode;
    }

    fclose(in);
    fclose(out);

    printf("Archivo descomprimido exitosamente.\n");
}