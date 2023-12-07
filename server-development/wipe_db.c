#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

int clear_directory(const char *dir_path) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(dir_path);
    if (dir == NULL) {
        perror("Error opening directory");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            // Skip "." and ".." entries
            continue;
        }

        char entry_path[PATH_MAX];
        sprintf(entry_path, "%s/%s", dir_path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            // Recursively clear subdirectories
            if (clear_directory(entry_path) == -1) {
                closedir(dir);
                return -1;
            }
            
            // Remove the empty directory
            if (rmdir(entry_path) == -1) {
                perror("Error removing directory");
                closedir(dir);
                return -1;
            }
        } else {
            // Remove the file
            if (remove(entry_path) == -1) {
                perror("Error removing file");
                closedir(dir);
                return -1;
            }
        }
    }

    closedir(dir);
    return 0;
}

int clear_bd() {
    if (clear_directory("USERS") == -1) {
        return -1;
    }

    if (clear_directory("AUCTIONS") == -1) {
        return -1;
    }

    return 0;
}

int main() {
    if (clear_bd() == 0) {
        printf("Directories cleared successfully.\n");
    } else {
        printf("Failed to clear directories.\n");
    }

    return 0;
}