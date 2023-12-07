#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FILE_SIZE 10 * 1024 * 1024  // 10 MB

int main() {
    FILE *file;
    const char* file_name = "large_file.txt";

    file = fopen(file_name, "wb");
    if (file == NULL) {
        printf("Error opening file.\n");
        return -1;
    }

    srand(time(NULL));
    for (int i = 0; i < FILE_SIZE; ++i) {
        char random_byte = rand() % 256;  // Generating a random byte (0-255)
        fwrite(&random_byte, sizeof(char), 1, file);
    }

    fclose(file);
    printf("File '%s' with size %d bytes created successfully.\n", file_name, FILE_SIZE);
    return 0;
}
