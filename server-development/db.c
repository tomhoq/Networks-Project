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

#define OK 10
#define NOK 11
#define REG 12
#define UNR 13

//FUNCOES BASE-------------------------------------------------------------------------------------------------------------------------------------------

int read_file(const char *file_path, char *buffer, int buffer_size) {
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        perror("Error opening file\n");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    if (buffer == NULL) {
        printf("Error allocating memory\n");
        fclose(fp);
        return -1;
    }
    if (buffer_size < file_size) {
        printf("WARNING: Buffer size is smaller than file size\n");
        fclose(fp);
        return -1;
    }
    if (fread(buffer, sizeof(char), buffer_size, fp) == -1) {
        perror("Error reading file\n");
        fclose(fp);
        free(buffer);
        return -1;
    }
    fclose(fp);
    return 1;
}

int file_exists(const char *file_path) {
    return access(file_path, F_OK) != -1;
}

int directory_exists(const char *path) {
    DIR *dir = opendir(path);

    if (dir != NULL) {
        closedir(dir);
        return 1;  // Directory exists
    } else {
        return 0;  // Directory doesn't exist or an error occurred
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
        printf("Error deleting file\n");
        return -1;  // Failed to delete file
    }
}

int create_directory(const char *dir_path) {
    int status = mkdir(dir_path, S_IRWXU | S_IRWXG);
    if (status == -1) {
        perror("Error creating directory\n");
        return -1;  // Failed to create directory
    }
    return 0;  // Directory created successfully
}

//-Funcoes complexas-----------------------------------------------------------------------------------------------------------------------------------------

int login_user(char username[10], char pass[10]) {
    char user_directory[30];
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
    if (strlen(username) != 6 || strlen(pass) != 8) {  
        printf("Username or password too long.\n");
        return -1;
    }

    // Create the user's directory
    sprintf(user_directory, "./USERS/%s/", username);


    // Check if the directory already exists
    if (!directory_exists(user_directory)) {
        // If the directory does not exist, create it
        if (create_directory(user_directory) == -1) {
            return -1;
        }

        sprintf(hosted_dir, "%sHOSTED/", user_directory);
        sprintf(bids_dir, "%sBIDDED/", user_directory);

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
    sprintf(pass_file, "%s%s_pass.txt", user_directory, username);  

    if (file_exists(pass_file)) {    // there is an account
        printf("File  already exists: %s\n", pass_file);
        char *p = malloc(10 * sizeof(char));
        memset(p, 0, 10);
        if (read_file(pass_file, p, 9) == -1) {
            printf("Error reading password file");
            return -1;
        }
        printf("%s\n",p);
        if (strcmp(p, pass) == 0) {
            printf("Password is correct.\n");
            exists = 1;
        } else {
            printf("Password is incorrect.\n");
            free(p);
            return NOK;
        }
        free(p);
    }
    else{ //registering user
        FILE* pass_fp = fopen(pass_file, "w");
        if (pass_fp == NULL) {
            printf("Error creating password file");
            return -1;
        }
        if (fwrite(pass, 1, strlen(pass), pass_fp) == -1) {  
            printf("Error writing password file");
            fclose(pass_fp);
            return -1;
        }
        fclose(pass_fp);
    }

    // Create the user's login file-------------------------------------------------------------------------------------------------------
    sprintf(login_file, "%s%s_login.txt", user_directory, username);

    if (file_exists(login_file)) {  //user is logged in already
        printf("User already logged in\n");
        return NOK;
    }
    sprintf(login_file, "%s/%s_login.txt", user_directory, username);
    printf("%s\n", login_file);
    FILE* login_fp = fopen(login_file, "w");
    if (login_fp == NULL) {
        printf("Error creating login file\n");
        return -1;
    }
    if (fwrite(username, 1, strlen(username), login_fp) == -1) {
        printf("Error writing login file\n");
        fclose(login_fp);
        return -1;
    }
    fclose(login_fp);

    return 0;
}

int logout(char username[10], char password[10]){

    // Verification
    if (strlen(username) != 6 || strlen(password) != 8) { 
        printf("Username or password too long.\n");
        return -1;
    }

    char user_directory[30];
    char pass_file[50];
    char login_file[50];

    sprintf(user_directory, "./USERS/%s/", username);
    sprintf(pass_file, "%s%s_pass.txt", user_directory, username);
    if (!directory_exists(user_directory)) {
        printf("Directory does not exist.\n");
        return UNR;
    } else if (!file_exists(pass_file)) {
        printf("Pass file does not exist\n");
        return UNR;
    }
    

}


int main() {
    char username[10] = "234234";
    char password[10] = "23423423";

    if (login_user("234234", "23423423") == 0) {
        printf("User registered successfully.\n");
    } else {
        printf("Failed to register user.\n");
    }

    return 0;
}
