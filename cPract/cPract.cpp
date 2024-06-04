#pragma warning(disable:4996)
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

char* read_string_from_stream(FILE* stream) {
    char* str = NULL;
    size_t len = 0;
    int c;

    while ((c = fgetc(stream)) != 0 && c != EOF) {
        str = (char*)realloc(str, len + 2); // Явное приведение типа
        switch (c)
        {
        case '\n':
            str[len++] = '<';
            str[len++] = 'l';
            str[len++] = 'f';
            str[len++] = '>';
            break;
        case '\r':
            str[len++] = '<';
            str[len++] = 'b';
            str[len++] = 'r';
            str[len++] = '>';
        default:
            str[len++] = (char)c;
        }
    }

    if (str) {
        str[len] = '\0';
    }

    return str;
}
int writeStringsToFile(char** strings, int stringCount, const char* fileName) {
    FILE* outFile = fopen(fileName, "w");
    if (outFile == NULL) {
        fprintf(stderr, "Failed to open output file: %s\n", fileName);
        return 1;
    }

    for (int i = 0; i < stringCount; i++) {
        fprintf(outFile, "%s\n", strings[i]);
    }

    fclose(outFile);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Использование: tool.exe <file.txt>\n");
        return 1;
    }

    FILE* f = fopen(argv[1], "rb");
    if (f == NULL) {
        printf("Не удалось открыть файл %s\n", argv[1]);
        return 1;
    }


    uint32_t Block1Size;
    uint32_t StringCount;
    fseek(f, 0x8, SEEK_SET);
    fread(&Block1Size, 4, 1, f);
    fread(&StringCount, 4, 1, f);
    fseek(f, Block1Size + 0x20, SEEK_SET);

    uint32_t* Offsets = (uint32_t*)malloc(StringCount * sizeof(uint32_t));
    char** strings = (char**)malloc(StringCount * sizeof(char*));

    for (int i = 0; i < StringCount; i++) {
        fread(&Offsets[i], 4, 1, f);
        
    }

    for (int i = 0; i < StringCount; i++) 
    {
        printf("Length of string %d: %d\n", i, (int)(Offsets[i + 1] - Offsets[i]));
        fseek(f, Offsets[i], SEEK_SET);
        strings[i] = read_string_from_stream(f);
        printf("%s\n", strings[i]);
    }
    char* filename = (char*)malloc(strlen(argv[1]) + 5); // Выделяем память для файла с .txt
    sprintf(filename, "%s.txt", argv[1]);
    writeStringsToFile(strings, StringCount, filename);
    free(strings);
    free(Offsets);
    free(filename);
    fclose(f);
    return 0;
}