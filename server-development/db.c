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

int is_password_correct(const char * file_path, char *password){
    char *p = malloc(10 * sizeof(char));
    memset(p, 0, 10);
    if (read_file(file_path, p, 9) == -1) {
        printf("Error reading password file");
        return -1;
    }
    printf("%s\n",p);
    if (strcmp(p, password) == 0) {
        printf("Password is correct.\n");
        free(p);
        return 1;
    } else {
        printf("Password is incorrect.\n");
        free(p);
        return 0;
    }
}

int is_auction_active(char *auction_id) {
    char auction_directory[30];
    char end_file[50];

    sprintf(auction_directory, "./AUCTIONS/%s/", auction_id);
    sprintf(end_file, "%sEND_%s.txt", auction_directory, auction_id);
    printf("auction_directory: %s\n", auction_directory);
    printf("end_file: %s\n", end_file);

    if (!directory_exists(auction_directory)) {
        printf("Directory does not exist\n");
        return -1;
    }
    else if (!file_exists(end_file)) {
        printf("Auction is active.\n");
        return 1;
    } else {
        printf("Auction is not active.\n");
        return 0;
    }
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

        if (is_password_correct(pass_file, pass)) {
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
    sprintf(login_file, "%s%s_login.txt", user_directory, username);
    if (!directory_exists(user_directory)) {
        printf("User not registered.\n");
        return UNR;
    } 
    if (!file_exists(pass_file)) {
        printf("User not registered. No pass_file\n");
        return UNR;
    }
    else if (!file_exists(login_file)) {
        printf("User not logged in.\n");
        return NOK;
    }
    else{
        if (is_password_correct(pass_file, password)) {
            printf("Password is correct.\n");
            delete_file(login_file);
            return OK;
        } else {
            printf("Password is incorrect.\n");
            return NOK;
        }
    }
}

int unregister(char username[10], char password[10]){

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
    sprintf(login_file, "%s%s_login.txt", user_directory, username);

    if (!directory_exists(user_directory)) {
        printf("User not registered.\n");
        return UNR;
    } 
    if (!file_exists(pass_file)) {
        printf("User not registered. No pass_file\n");
        return UNR;
    }
    else if (!file_exists(login_file)) {
        printf("User not logged in.\n");
        return NOK;
    }
    else{
        if (is_password_correct(pass_file, password)) {
            printf("Password is correct.\n");
            delete_file(login_file);
            delete_file(pass_file);
            return OK;
        } else {
            printf("Password is incorrect.\n");
            return NOK;
        }
    }
}

char *get_user_auctions(char username[10]){
    char user_directory[30];
    char login_file[50];
    char hosted_dir[50];
    DIR *dir;
    struct dirent *entry;

    sprintf(user_directory, "./USERS/%s/", username);
    sprintf(login_file, "%s%s_login.txt", user_directory, username);
    sprintf(hosted_dir, "%sHOSTED/", user_directory);

    if (!directory_exists(user_directory)) {
        printf("User not registered.\n");
        return NULL;
    }
    else if (!directory_exists(hosted_dir)) {
        printf("This should not happen. ERROR AT DB\n");
        return NULL;
    }
    else if (!file_exists(login_file)) {
        printf("User not logged in.\n");
        return NULL;
    }
    else{
        int number_files = 0;
        char *auctions;
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir (hosted_dir)) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                    number_files += 1;
                }
            }
            if (closedir(dir) == -1) {
                printf("Error closing directory\n");
                return NULL;
            }
        }

        if (number_files == 0) {
            printf("No auctions hosted.\n");
            return NULL;
        }
        auctions = malloc(number_files * 7 * sizeof(char));
        memset(auctions, '\0', 100);
        char auc[5];
        memset(auc, '\0', 5);

        if ((dir = opendir (hosted_dir)) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                number_files -= 1;
                if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                    char *dotPosition = strchr(ent->d_name, '.');
                    // Copy only the part before the period (if any)
                    if (dotPosition != NULL) {
                        strncat(auc, ent->d_name, dotPosition - ent->d_name);
                        strcat(auctions, auc);
                        if (is_auction_active(auc)) {
                            strcat(auctions, " 1");
                        } else {
                            strcat(auctions, " 0");
                        }
                        printf("b: %s\n, %d\n", ent->d_name, dotPosition - ent->d_name);
                    } else {
                        return NULL;
                    }
                    if (number_files != 0)
                        strcat(auctions, " ");
                    else
                        strcat(auctions, "\n");
                    memset(auc, '\0', 5);
                }
            }
            if (closedir(dir) == -1) {
                printf("Error closing directory\n");
                free(auctions);
                return NULL;
            }
        } else {
            free(auctions);
            printf("Error opening directory\n");
            return NULL;
        }
        return auctions;
    }
}

int main() {
    char username[10] = "234234";
    char password[10] = "23423423";

    
    if (unregister("234234", "23423423") == 0) { // corrigir!
        printf("User login successfully.\n");
    } 
    /*if
    char *s = get_user_auctions("234234");  // corrigir!
    printf("Auctions: %s\n", s);
    free(s);

    s = get_user_auctions("123123");
    printf("Auctions: %s\n", s);
    if (s == NULL) {
        printf("0 auctions.\n");
    }


    //sleep(1);
    
    printf("logging out\n");
    printf("%d\n", logout("234234", "23423423"));
    printf("%d\n", unregister("234234", "23423423"));*/

    return 0;
}
