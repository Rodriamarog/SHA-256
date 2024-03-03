#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include "sha256.h"

void print_hash(BYTE hash[]) {
    for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

void hash_file(const char *filepath, FILE *outputCsv) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        perror("File opening failed");
        return;
    }

    SHA256_CTX ctx;
    BYTE buf[1024];
    BYTE hash[SHA256_BLOCK_SIZE];
    char hashString[SHA256_BLOCK_SIZE * 2 + 1]; // Each byte as two hex characters

    sha256_init(&ctx);

    size_t bytes_read;
    while ((bytes_read = fread(buf, 1, sizeof(buf), file)) > 0) {
        sha256_update(&ctx, buf, bytes_read);
    }

    sha256_final(&ctx, hash);

    for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
        sprintf(&hashString[i*2], "%02x", hash[i]);
    }

    struct stat st;
    stat(filepath, &st); // Get file size

    // Write hash, filename, and size to CSV
    fprintf(outputCsv, "%s,%s,%ld\n", hashString, filepath, st.st_size);

    fclose(file);
}

int main() {
    const char *directoryPath = "/home/rodrigo/WebDev/estatica";
    const char *outputCsvPath = "/home/rodrigo/WebDev/estatica/output.csv"; // Specify the CSV file path
    FILE *outputCsv = fopen(outputCsvPath, "w"); // Open the CSV file for writing
    if (!outputCsv) {
        perror("Failed to open output CSV");
        return 1;
    }

    DIR *dir = opendir(directoryPath);
    if (dir == NULL) {
        perror("Directory open failed");
        fclose(outputCsv); // Ensure to close the CSV file if directory opening fails
        return 1;
    }

    struct dirent *entry;
    struct stat file_stat;
    char filepath[1024];

    while ((entry = readdir(dir)) != NULL) {
        snprintf(filepath, sizeof(filepath), "%s/%s", directoryPath, entry->d_name);

        if (stat(filepath, &file_stat) == 0) {
            if (S_ISREG(file_stat.st_mode)) {
                printf("Hashing: %s\n", filepath);
                hash_file(filepath, outputCsv); // Pass the CSV file stream to hash_file
            }
        }
    }

    closedir(dir);
    fclose(outputCsv); // Close the CSV file after all files have been processed
    return 0;
}

