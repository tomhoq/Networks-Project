#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

typedef struct prb {  //protocolo resposta da base de dados
    int return_code;
    char[50] error_message;
    double third;
};  // substituir todas as funcoes para devolver ret


#define OK 10
#define NOK 11
#define REG 12

char* read_file(const char *file_path) {
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    char *file_contents = malloc(file_size * (sizeof(char)));
    if (file_contents == NULL) {
        printf("Error allocating memory");
        fclose(fp);
        return -1;
    }

    if (fread(file_contents, sizeof(char), file_size, fp) == -1) {
        perror("Error reading file");
        fclose(fp);
        free(file_contents);
        return -1;
    }

    fclose(fp);
    return file_contents;
}

int is_file_exists(const char *file_path) {
    return access(file_path, F_OK) != -1;
}

int is_directory_exists(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (dir) {
        closedir(dir);
        return 1;  // Directory exists
    } else {
        return -1;  // Directory does not exist or there was an error
    }
}

int delete_file(const char *file_path) {
    if (!file_exists(file_path)) {
        printf("File does not exist: %s\n", file_path);
        return -1;  // File does not exist
    }

    if (remove(file_path) == 0) {
        printf("File deleted successfully: %s\n", file_path);
        return 1;  // File deleted successfully
    } else {
        printf("Error deleting file");
        return -1;  // Failed to delete file
    }
}

int create_directory(const char *dir_path) {
    int status = mkdir(dir_path, S_IRWXU | S_IRWXG);
    if (status == -1) {
        perror("Error creating directory");
        return -1;  // Failed to create directory
    }
    return 0;  // Directory created successfully
}

int login_user(char username[10], char pass[10]) {
    char user_directory[15];
    char pass_file[50];
    char login_file[50];
    char hosted_dir[50];
    char bids_dir[50];
    int exists = 0;

    memset(hosted_dir, 0, sizeof(hosted_dir));
    memset(bids_dir, 0, sizeof(bids_dir));
    memset(user_directory, 0, sizeof(user_directory));
    memset(pass_file, 0, sizeof(pass_file));
    memset(login_file, 0, sizeof(login_file));

    // Verification
    if (strlen(username) != 6 || strlen(pass) != 8) {  // Fix: Use '==' for equality check
        printf("Username or password too long.\n");
        return -1;
    }

    // Create the user's directory
    sprintf(user_directory, "USERS/%s/", username);


    // Check if the directory already exists
    if (!is_directory_exists(user_directory)) {
        // If the directory does not exist, create it
        if (create_directory(user_directory) == -1) {
            return -1;
        }

        sprintf(hosted_dir, "%s/HOSTED/", user_directory);
        sprintf(bids_dir, "%s/BIDDED/", user_directory);

        if (create_directory(hosted_dir) == -1) {
            return -1;
        }
        if (create_directory(bids_dir) == -1) {
            return -1;
        }
    } else {
        printf("Directory already exists.\n"); //user is going to reset password
    }
    
    // Create the user's password file-------------------------------------------------------------------------------------------------------
    sprintf(pass_file, "%s/%s_pass.txt", user_directory, username);  
    if (is_file_exists(pass_file)) {
        printf("File  already exists: %s\n", pass_file);
        char *p = read_file(pass_file);
        if (strcmp(p, pass) == 0) {
            printf("Password is correct.\n");
            exists = 1;
        } else {
            printf("Password is incorrect.\n");
            return NOK;
        }
    }
    else{ //registering user
        FILE* pass_fp = fopen(pass_file, "w");
        if (pass_fp == NULL) {
            perror("Error creating password file");
            return NOK;
        }
        if (fwrite(pass, 1, strlen(pass), pass_fp) == -1) {  // Fix: Correct the arguments of fwrite
            perror("Error writing password file");
            fclose(pass_fp);
            return NOK;
        }
        fclose(pass_fp);
    }

    // Create the user's login file-------------------------------------------------------------------------------------------------------
    sprintf(login_file, "%s/%s_login.txt", user_directory, username);

    if (is_file_exists(login_file)) {
        printf("File  already exists: %s\n", pass_file);
        return NOK;
    }
    sprintf(login_file, "%s/%s_login.txt", user_directory, username);
    FILE* login_fp = fopen(login_file, "w");
    if (login_fp == NULL) {
        perror("Error creating login file");
        return -1;
    }
    if (fwrite(username, 1, strlen(username), login_fp) == -1) {
        perror("Error writing login file");
        fclose(login_fp);
        return -1;
    }
    fclose(login_fp);

    return 0;
}

int logout(char username[10], char password[10]){

    // Verification
    if (strlen(username) != 6 || strlen(pass) != 8) { 
        printf("Username or password too long.\n");
        return -1;
    }

    

}


int main() {
    char username[10] = "234234";
    char password[10] = "23423423";

    if (register_user(username, password) == 0) {
        printf("User registered successfully.\n");
    } else {
        printf("Failed to register user.\n");
    }
    printf("adawd\n");
    if (register_user(username, password) == 0) {
        printf("User registered successfully.\n");
    } else {
        printf("Failed to register user.\n");
    }

    return 0;
}
