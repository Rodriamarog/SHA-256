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

void hash_file(const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        perror("File opening failed");
        return;
    }

    SHA256_CTX ctx;
    BYTE buf[1024];
    BYTE hash[SHA256_BLOCK_SIZE];

    sha256_init(&ctx);

    size_t bytes_read;
    while ((bytes_read = fread(buf, 1, sizeof(buf), file)) > 0) {
        sha256_update(&ctx, buf, bytes_read);
    }

    sha256_final(&ctx, hash);
    print_hash(hash);

    fclose(file);
}

int main() {
    const char *directoryPath = "/home/rodrigo/WebDev/estatica";
    DIR *dir = opendir(directoryPath);
    if (dir == NULL) {
        perror("Directory open failed");
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
                hash_file(filepath);
            }
        }
    }

    closedir(dir);
    return 0;
}
