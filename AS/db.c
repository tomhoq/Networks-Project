#include "wipe_db.c"
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
#include <time.h>
#include <dirent.h>

#define OK 10
#define NOK 11
#define REG 12
#define UNR 13
#define NLG 14
#define EAU 15
#define EOW 16
#define END 17
#define ILG 18
#define REF 19
#define ACC 20

#define last 666
#define UNITIALIZED 667

typedef struct {
    char bidder_UID[10];
    char bid_value[10];
    char bid_date[20];
    char bid_time[20];
    char bid_sec_time[15];
    int last_bid;
} bid;

typedef struct {
    char id[10];
    char host_uid[10];
    char auction_name[20];
    char asset_name[30];
    char start_value[10];
    char start_date[20];
    char start_time[20];
    char time_active[10];
    char start_fulltime[15];
    
    bid bids[51];
    char end_date[20];
    char end_time[20];
    char end_sec_time[10];
    int active;
} auction;

//FUNCOES BASE-------------------------------------------------------------------------------------------------------------------------------------------

int create_file(const char *file_path, char *buffer) {
    FILE* file = fopen(file_path, "w");
    if (file == NULL) {
        printf("Error creating password file");
        return -1;
    }
    if (fwrite(buffer, 1, strlen(buffer), file) == -1) {  
        printf("Error writing password file");
        fclose(file);
        return -1;
    }
    fclose(file);
    return 1;
}
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

//returns a string with how many seconds have passed since UNIX EPOCH, 1970
char* get_seconds_elapsed() {
    time_t current_time;
    time(&current_time);
    char *time_string = malloc(12 * sizeof(char));
    snprintf(time_string, 12, "%ld", (long)current_time);

    return time_string;
}

char* get_date() {
    time_t fulltime;
    struct tm *current_time;
    char *time_string = malloc(25 * sizeof(char));
    memset(time_string, '\0', 25);

    time(&fulltime);

    current_time = gmtime(&fulltime);

    sprintf(time_string,"%4d-%02d-%02d %02d:%02d:%02d", current_time->tm_year + 1900, current_time->tm_mon + 1, current_time->tm_mday, 
                                                        current_time->tm_hour, current_time->tm_min, current_time->tm_sec);

    return time_string;
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
        //printf("File deleted successfully: %s\n", file_path);
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

int folder_exists(const char *dir_path) {
    DIR *dir = opendir(dir_path);

    if (dir != NULL) {
        closedir(dir);
        return 1;  // Directory exists
    } else {
        return 0;  // Directory doesn't exist or an error occurred
    }
}

int is_password_correct(const char * file_path, char *password){
    char *p = malloc(10 * sizeof(char));
    memset(p, 0, 10);
    if (read_file(file_path, p, 9) == -1) {
        printf("Error reading password file");
        return -1;
    }
    //printf("%s vs %s\n", p, password);

    if (strcmp(p, password) == 0) {
        //printf("Password is correct.\n");
        free(p);
        return 1;
    } else {
        printf("Password is incorrect. %s vs %s\n", p, password);
        free(p);
        return 0;
    }
}

int get_most_recent_aid(char buffer[5]) {
    char auctions_dir[30];
    struct dirent **filelist;

    memset(buffer, '\0', 5);

    strcpy(auctions_dir, "./AS/AUCTIONS/");

    if (!directory_exists(auctions_dir)) {
        printf("ERROR WITH DB.\n");
        return -1;
    }
    int number_auctions = scandir(auctions_dir, &filelist, 0, alphasort);
    if (number_auctions <= 0)
        return -1;

    int j = 0;
    strncpy(buffer, filelist[number_auctions-1]->d_name, 3);
    free(filelist);
    return 1;
}

char* get_auction_path(char aid[5]) {
    char *path = malloc(50 * sizeof(char));
    memset(path, '\0', 50);
    sprintf(path, "./AS/AUCTIONS/%s/ASSET/", aid);
    return path;
}

//devolve 1 se tiver ativo e 0 senao
int check_validity(char *auction_id){
    char start_file[50];
    char auction_directory[30];
    char end_file[50];

    sprintf(auction_directory, "./AS/AUCTIONS/%s/", auction_id);
    sprintf(start_file, "%sSTART_%s.txt", auction_directory, auction_id);   

    if (!directory_exists(auction_directory)) {
        //printf("Directory does not exist\n");
        return -1;
    } else if (!file_exists(start_file)) {
        printf("Start file does not exist\n");
        return -1;
    } else {
        char buffer[140];
        memset(buffer, '\0', sizeof(buffer));
        if (read_file(start_file, buffer, sizeof(buffer)-1) == -1) {
            printf("Error reading start file");
            return -1;
        }
        if (strlen(buffer) == 0){
            printf("ERROR AT DB. Empty Start file.\n");
            return -1;
        }
        char *start_fulltime = malloc(15 * sizeof(char));
        memset(start_fulltime, '\0', 15);
        char *time_active = malloc(15 * sizeof(char));
        memset(time_active, '\0', 15);
        if (sscanf(buffer, "%*s %*s %*s %*s %s %*s %*s %s", time_active, start_fulltime) != 2){
            printf("ERROR AT DB-> Wrong content in start file->\n");
            return -1;
        }

        // convert to int and compare with current time
        char *secs = get_seconds_elapsed();
        int time_active_int = atoi(time_active);
        int start_fulltime_int = atoi(start_fulltime);
        int current_time_int = atoi(secs);

        free(start_fulltime);
        free(time_active);
        free(secs);

        //printf("time_active_int: %d\n", time_active_int);
        //printf("start_fulltime_int: %d\n", start_fulltime_int);
        //printf("old?: %d\n", current_time_int - (start_fulltime_int + time_active_int));

        if (current_time_int > (start_fulltime_int + time_active_int)) {
            //printf("Auction is not active.\n");
            sprintf(end_file, "%sEND_%s.txt", auction_directory, auction_id);
            memset(buffer, '\0', sizeof(buffer));
            
            time_t fulltime = start_fulltime_int + time_active_int;
            struct tm *current_time = localtime(&fulltime);
            char time_str[25];


            if (current_time != NULL) {
                sprintf(time_str,"%4d-%02d-%02d %02d:%02d:%02d",
                        current_time->tm_year + 1900, current_time->tm_mon + 1, current_time->tm_mday, 
                        current_time->tm_hour, current_time->tm_min, current_time->tm_sec);

                sprintf(buffer, "%s %d", time_str, (start_fulltime_int + time_active_int));

                //printf("buffer: %s\n", buffer);

                if (create_file(end_file, buffer) == -1) {
                    printf("Error creating end file");
                    return -1;
                }
            } else {
                printf("Error converting time.\n");
                return -1;
            }

            return 0;
        } else {
            //printf("Auction is active.\n");
            return 1;
        }

        return 1;
    }
}

//devolve 1 se activo, 0 se inativo
int is_auction_active(char *auction_id) {
    char auction_directory[30];
    char end_file[50];

    sprintf(auction_directory, "./AS/AUCTIONS/%s/", auction_id);
    sprintf(end_file, "%sEND_%s.txt", auction_directory, auction_id);
    //printf("auction_directory: %s\n", auction_directory);
    //printf("end_file: %s\n", end_file);

    if (!directory_exists(auction_directory)) {
        printf("Directory does not exist\n");
        return -1;
    }
    else if (!file_exists(end_file)) {
        //verificar se tempo ja passou
        int n;
        if ((n = check_validity(auction_id)) == 1) {
            //printf("Auction is active.\n");
            return 1;
        } else if (n == 0)  {
            printf("Auction is not active.\n");
            return 0;
        } else {
            printf("Error checking validity\n");
        }
    } else {
        printf("Auction is not active.\n");
        return 0;
    }
}

char* get_asset_name(char aid[5]) {
    char auction_directory[30];
    char start_file[50];
    char buffer[140];
    char *asset_name = (char *) malloc(30 * sizeof(char));
    memset(asset_name, '\0', 30);
    
    sprintf(auction_directory, "./AS/AUCTIONS/%s/", aid);
    sprintf(start_file, "%sSTART_%s.txt", auction_directory, aid);   

    if (!directory_exists(auction_directory)) {
        printf("Directory does not exist\n");
        return NULL;
    } else if (!file_exists(start_file)) {
        printf("Start file does not exist\n");
        return NULL;
    } else {
        memset(buffer, '\0', sizeof(buffer));
        if (read_file(start_file, buffer, sizeof(buffer)-1) == -1) {
            printf("Error reading start file");
            return NULL;
        }
        if (strlen(buffer) == 0){
            printf("ERROR AT DB. Empty Start file.\n");
            return NULL;
        }
        if (sscanf(buffer, "%*s %*s %s", asset_name) != 1){
            printf("ERROR AT DB-> Wrong content in start file->\n");
            return NULL;
        }
        return asset_name;
    }
}

int is_bid_greater(char aid[5], char bid_value[10]){
    char bids_dir[50];
    char start_file[60];
    char current_bid[10];
    struct dirent **filelist;
    memset(current_bid, '\0', sizeof(current_bid));
    memset(bids_dir, '\0', sizeof(bids_dir));
    memset(start_file, '\0', sizeof(start_file)); 

    sprintf(bids_dir, "./AS/AUCTIONS/%s/BIDS/", aid);
    sprintf(start_file, "./AS/AUCTIONS/%s/START_%s.txt", aid, aid);

    int number_files = scandir(bids_dir, &filelist, 0, alphasort);
    if (number_files <= 0)
        return -1;
    if (number_files == 2) { //no bids placed
        char buffer[140];
        if (read_file(start_file, buffer, sizeof(buffer)-1) == -1) {
            printf("Error reading start file");
            return -1;
        }
        sscanf(buffer, "%*s %*s %*s %s", current_bid);        
    }
    else {
        strncpy(current_bid, filelist[number_files-1]->d_name, 6);
        
    }
    //printf("current_bid: %s\n", current_bid);

    if (atoi(bid_value) > atoi(current_bid)) {
            for (int i = 0; i < number_files; ++i) {
                free(filelist[i]);
            }
            free(filelist);
            return 1;
    } 
    else {
        printf("Bid is not greater than current bid.\n");
        for (int i = 0; i < number_files; ++i) {
            free(filelist[i]);
        }
        free(filelist);
        return 0;
    }
}

int delete_auction(char aid[5], char uid[10]){
    char auctions_dir[30];
    char a_dir[50];
    char bids_dir[60];
    char asset_dir[60];
    char user_directory[30];
    char auction_file[50];
    char pass_file[50];
    char hosted_file[50];
    int len;
    struct dirent **filelist;

    sprintf(user_directory, "./AS/USERS/%s/", uid);
    sprintf(hosted_file, "%sHOSTED/%s.txt", user_directory, aid);

    sprintf(auction_file, "./AS/AUCTIONS/%s/", aid);

    delete_all(auction_file);
    delete_file(hosted_file);
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
    sprintf(user_directory, "./AS/USERS/%s/", username);


    // Check if the directory already exists
    if (!directory_exists(user_directory)) {
        // If the directory does not exist, create it
        if (create_directory(user_directory) == -1) {
            return -1;
        }

        sprintf(hosted_dir, "%sHOSTED/", user_directory);
        sprintf(bids_dir, "%sBIDDED/", user_directory);
        
        if (create_directory(hosted_dir) == -1) {
            delete_all(user_directory);
            return -1;
        }
        if (create_directory(bids_dir) == -1) {
            delete_all(user_directory);
            return -1;
        }
    } else {
        //printf("Directory already exists.\n"); //user is going to reset password

        //temporario
        sprintf(hosted_dir, "%sHOSTED/", user_directory);
        sprintf(bids_dir, "%sBIDDED/", user_directory);
        if (!directory_exists(hosted_dir)) {
            if (create_directory(hosted_dir) == -1) {
                delete_all(user_directory);
                return -1;
            }
        }
        if (!directory_exists(bids_dir)) {
            if (create_directory(bids_dir) == -1) {
                delete_all(user_directory);
                return -1;
            }
        }
    }
    
    // Create the user's password file-------------------------------------------------------------------------------------------------------
    sprintf(pass_file, "%s%s_pass.txt", user_directory, username);  

    if (file_exists(pass_file)) {    // there is an account
        //printf("File already exists: %s\n", pass_file);

        if (!is_password_correct(pass_file, pass)) {
            return NOK;
        }
        exists = 1;
    }
    else{ //registering user
        if (create_file(pass_file, pass) == -1) {
            return NOK;
        }
    }

    // Create the user's login file-------------------------------------------------------------------------------------------------------
    sprintf(login_file, "%s%s_login.txt", user_directory, username);

    if (file_exists(login_file)) {  //user is logged in already
        printf("User already logged in.\n");
        return NOK;
    }
    //printf("%s\n", login_file);
    
    if (create_file(login_file, username) == -1) {  
        return NOK;
    }
    //printf("adaw\n");
    if (!exists)
        return REG;
    return OK;
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

    sprintf(user_directory, "./AS/USERS/%s/", username);
    sprintf(pass_file, "%s%s_pass.txt", user_directory, username);
    sprintf(login_file, "%s%s_login.txt", user_directory, username);
    if (!directory_exists(user_directory)) {
        printf("User not registered.\n");
        return UNR;
    } 
    if (!file_exists(pass_file)) {
        printf("User not registered. No pass_file.\n");
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

    sprintf(user_directory, "./AS/USERS/%s/", username);
    sprintf(pass_file, "%s%s_pass.txt", user_directory, username);
    sprintf(login_file, "%s%s_login.txt", user_directory, username);

    if (!directory_exists(user_directory)) {
        printf("User not registered.\n");
        return UNR;
    } 
    if (!file_exists(pass_file)) {
        printf("User not registered. No pass_file.\n");
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

int get_user_auctions(char username[10], auction list[1000]){
    char user_directory[30];
    char login_file[50];
    char hosted_dir[50];
    int len;
    struct dirent **filelist;

    sprintf(user_directory, "./AS/USERS/%s/", username);
    sprintf(login_file, "%s%s_login.txt", user_directory, username);
    sprintf(hosted_dir, "%sHOSTED/", user_directory);

    if (!directory_exists(user_directory)) {
        printf("User not registered.\n");
        return NLG;
    }
    else if (!directory_exists(hosted_dir)) {
        printf("This should not happen. ERROR AT DB.\n");
        return NOK;
    }
    else if (!file_exists(login_file)) {
        printf("User not logged in.\n");
        return NLG;
    }
    else {
        int number_auctions = scandir(hosted_dir, &filelist, 0, alphasort);
        if (number_auctions <= 0)
            return NOK;

        int j = 0, i = 0;
        char id[4];
        char end_file[50];
        while(j<number_auctions){
            memset(id, 0, sizeof(id));
            memset(end_file, 0, sizeof(end_file));
            len = strlen(filelist[j]->d_name);
            if (len == 7) {
                const char *lastDot = strrchr(filelist[j]->d_name, '.');

                if (lastDot != NULL) {
                    // Found a dot, extract basename and extension
                    strncpy(id, filelist[j]->d_name, lastDot - filelist[j]->d_name);
                    strcpy(list[i].id, id);
    
                    check_validity(id);  //closes auction if duration has finished

                    sprintf(end_file, "./AS/AUCTIONS/%s/END_%s.txt", id, id);
                    if (file_exists(end_file)) {
                        list[i].active = 0;
                    } else {
                        list[i].active = 1;
                    }
                    
                    i++;

                }
            }
            free(filelist[j]);
            j++;
        }
        free(filelist);
        
        list[i].active = last;

        if (i ==0)
            return NOK;
        else
            return OK;
    }

}

int get_user_bids(char username[10], auction list[1000]){
    char user_directory[30];
    char login_file[50];
    char bidded_dir[50];
    int len;
    struct dirent **filelist;

    sprintf(user_directory, "./AS/USERS/%s/", username);
    sprintf(login_file, "%s%s_login.txt", user_directory, username);
    sprintf(bidded_dir, "%sBIDDED/", user_directory);

    if (!directory_exists(user_directory)) {
        printf("User not registered.\n");
        return NLG;
    }
    else if (!directory_exists(bidded_dir)) {
        printf("This should not happen. ERROR AT DB\n");
        return NOK;
    }
    else if (!file_exists(login_file)) {
        printf("User not logged in.\n");
        return NLG;
    }
    else {
        int number_auctions = scandir(bidded_dir, &filelist, 0, alphasort);
        if (number_auctions <= 0)
            return NOK;

        int j = 0, i = 0;
        char id[4];
        char end_file[50];
        while(j<number_auctions){
            memset(id, 0, sizeof(id));
            memset(end_file, 0, sizeof(end_file));
            len = strlen(filelist[j]->d_name);
            if (len == 7) {
                const char *lastDot = strrchr(filelist[j]->d_name, '.');

                if (lastDot != NULL) {
                    // Found a dot, extract basename and extension
                    strncpy(id, filelist[j]->d_name, lastDot - filelist[j]->d_name);
                    strcpy(list[i].id, id);

                    check_validity(id);  //closes auction if duration has finished


                    sprintf(end_file, "./AS/AUCTIONS/%s/END_%s.txt", id, id);
                    if (file_exists(end_file)) {
                        list[i].active = 0;
                    } else {
                        list[i].active = 1;
                    }
                    
                    i++;

                }
            }
            free(filelist[j]);
            j++;
        }
        free(filelist);
        
        list[i].active = last;

        if (i ==0)
            return NOK;
        else
            return OK;
    }

}

int get_all_auctions(auction list[1000]){
    char auctions_dir[30];
    char login_file[50];
    char bidded_dir[50];
    int len;
    struct dirent **filelist;

    strcpy(auctions_dir, "./AS/AUCTIONS/");

    if (!directory_exists(auctions_dir)) {
        printf("ERROR WITH DB.\n");
        return NOK;
    }
    else {
        int number_auctions = scandir(auctions_dir, &filelist, 0, alphasort);
        if (number_auctions <= 0)
            return NOK;

        int j = 0, i = 0;
        char id[4];
        char end_file[50];
        while(j<number_auctions){
            memset(end_file, '\0', sizeof(end_file));
            memset(id, '\0', sizeof(id));
            len = strlen(filelist[j]->d_name);
            if (len == 3) {
                //printf("%s\n", filelist[j]->d_name);
                strncpy(id, filelist[j]->d_name, 3);
                strcpy(list[i].id, id);

                check_validity(id);  //closes auction if duration has finished

                sprintf(end_file, "./AS/AUCTIONS/%s/END_%s.txt", id, id);
                if (file_exists(end_file)) {
                    list[i].active = 0;
                } else {
                    list[i].active = 1;
                }
                
                i++;

            }
            free(filelist[j]);
            j++;
        }
        free(filelist);
        
        list[i].active = last;
        //printf("Number of auctions: %d\n", i);
        if (i ==0)
            return NOK;
        else
            return OK;
    }

}

// para chamar enviar um endereco para uma auction como:        get_record("001", &a);!!!!!
int get_record(char aid[5], auction *a){
    char auctions_dir[30];
    char a_dir[50];
    char bids_dir[70];
    char start_file[70];
    char b_dir[90];
    int len;
    struct dirent **filelist;

    strcpy(auctions_dir, "./AS/AUCTIONS/");
    sprintf(a_dir, "%s%s/", auctions_dir, aid);

    check_validity(aid);  //closes auction if duration has finished

    if (!directory_exists(auctions_dir)) {
        printf("ERROR WITH DB.\n");
        return NOK;
    }
    if (!directory_exists(a_dir)) {
        printf("Auction does not exist.\n");
        return NOK;
    }
    else {
        sprintf(start_file, "%sSTART_%s.txt", a_dir, aid);
        if (!file_exists(start_file)){
            printf("ERROR AT DB. No start file found.\n");
            return NOK;
        }
        char buffer[140];
        memset(buffer, '\0', sizeof(buffer));
        if (read_file(start_file, buffer, sizeof(buffer)-1) == -1) {
            printf("Error reading start file");
            return NOK;
        }
        if (strlen(buffer) == 0){
            printf("ERROR AT DB. Empty Start file.\n");
            return NOK;
        }
        strcpy(a->id, aid);
        if (sscanf(buffer, "%s %s %s %s %s %s %s %s", a->host_uid, a->auction_name,
        a->asset_name, a->start_value, a->time_active, a->start_date, a->start_time, a->start_fulltime) != 8){
            printf("ERROR AT DB-> Wrong content in start file->\n");
            return NOK;
        }

        //printf("%s %s %s %s %s %s %s %s\n", a->host_uid, a->auction_name, a->asset_name, a->start_value, a->time_active, a->start_date, a->start_time, a->start_fulltime);

        sprintf(bids_dir, "%sBIDS/", a_dir);
        //printf("bids_dir: %s\n", bids_dir);
        if (!directory_exists(bids_dir)){
            printf("ERROR AT DB. No bids directory found.\n");
            return NOK;
        }

        int number_bids = scandir(bids_dir, &filelist, 0, alphasort);
        int surplus = 0;

        if (number_bids <= 0)
            return NOK;
        
        if (number_bids > 52) {
            surplus = number_bids - 50;
        }

        int j = 0, i = 0;
        char bid_name[15];
        while (j<number_bids){
            len = strlen(filelist[j]->d_name);
            if (len == 10 && j >= surplus) {
                memset(buffer, '\0', sizeof(buffer));
                memset(b_dir, '\0', sizeof(b_dir));
                memset(bid_name, '\0', sizeof(bid_name));
                strncpy(bid_name, filelist[j]->d_name, sizeof(bid_name)-1);
                sprintf(b_dir, "%s%s", bids_dir, bid_name);
                if (read_file(b_dir, buffer, sizeof(buffer)-1) == -1) {
                    printf("Error reading bid file");
                    return NOK;
                }
                if (strlen(buffer) == 0){
                    printf("ERROR AT DB. Empty bid file.\n");
                    return NOK;
                }
                if (sscanf(buffer, "%s %s %s %s %s", a->bids[i].bidder_UID, a->bids[i].bid_value, a->bids[i].bid_date, a->bids[i].bid_time, a->bids[i].bid_sec_time) != 5){
                    printf("ERROR AT DB. Wrong content in bid file.\n");
                    return NOK;
                }
                a->bids[i].last_bid = 0;

                //printf("%s %s %s %s %s\n", a->bids[i].bidder_UID, a->bids[i].bid_value, a->bids[i].bid_date, a->bids[i].bid_time, a->bids[i].bid_sec_time);
                i++;
            }
            free(filelist[j]);
            j++;
        }
        free(filelist);
        //printf("bid to be last: %d\n", i);
        a->bids[i].last_bid = last;

        char end_file[70];
        sprintf(end_file, "%sEND_%s.txt", a_dir, aid);
        if (!file_exists(end_file)) {
            a->active = 1;
        } else {   //ficheiro existe logo ir ler
            a->active = 0;   

            memset(buffer, '\0', sizeof(buffer));
            if (read_file(end_file, buffer, sizeof(buffer)-1) == -1) {
                printf("Error reading end file");
                return NOK;
            }
            if (strlen(buffer) == 0) {
                printf("ERROR AT DB. Empty end file.\n");
                return NOK;
            }
            if (sscanf(buffer, "%s %s %s", a->end_date, a->end_time, a->end_sec_time) != 3){
                printf("ERROR AT DB. Wrong content in end file.\n");
                return NOK;
            }
            //printf("%s %s %s\n", a->end_date, a->end_time, a->end_sec_time);
        }
        
        return OK;
            
    }
}

// chamar create auction e depois enter asset
int create_auction(char uid[8], char password[10], char auction_name[20], char start_value[10],  char time_active[10], char file_name[30]) {
    char auctions_dir[30];
    char a_dir[50];
    char bids_dir[60];
    char asset_dir[60];
    char aid[4];
    char user_directory[30];
    char login_file[50];
    char pass_file[50];
    char hosted_dir[50];
    int len;
    struct dirent **filelist;

    sprintf(user_directory, "./AS/USERS/%s/", uid);
    sprintf(login_file, "%s%s_login.txt", user_directory, uid);
    sprintf(pass_file, "%s%s_pass.txt", user_directory, uid);
    sprintf(hosted_dir, "%sHOSTED/", user_directory);


    //check login------------------------------------------------
    if (!directory_exists(user_directory)) {
        printf("User not registered.\n");
        return NLG;
    }
    else if (!directory_exists(hosted_dir)) {
        printf("This should not happen. ERROR AT DB.\n");
        return NOK;
    }
    else if (!file_exists(login_file)) {
        printf("User not logged in.\n");
        return NLG;
    }
    else if (!file_exists(pass_file)) {
        printf("User is not registered.\n");
        return NLG;
    }
    else if(is_password_correct(pass_file, password) == 0) {
        return NOK;
    }

    //check auctions------------------------------------------------
    strcpy(auctions_dir, "./AS/AUCTIONS/");

    if (!directory_exists(auctions_dir)) {
        printf("ERROR WITH DB.\n");
        return NOK;
    }
    int number_auctions = scandir(auctions_dir, &filelist, 0, alphasort);
    if (number_auctions <= 0)
        return NOK;

    for (int i = 0; i < number_auctions; ++i) {
        free(filelist[i]);
    }
    free(filelist);

    number_auctions -= 2;
    //printf("number of auctions: %d\n", number_auctions);
    if (number_auctions >= 999) {
        printf("Maximum number of auctions reached.\n");
        return NOK;
    }

    snprintf(aid, 4, "%03d", number_auctions +1);
    sprintf(a_dir, "%s%s/", auctions_dir, aid);
    sprintf(bids_dir, "%sBIDS/", a_dir);
    sprintf(asset_dir, "%sASSET/", a_dir);

    if (directory_exists(a_dir)) {
        printf("ERROR WITH DB. Auction already exists.\n");
        return NOK;
    }
    if (create_directory(a_dir) == -1) {
        return NOK;
    }
    if (create_directory(bids_dir) == -1) {
        delete_all(a_dir);
        return NOK;
    }
    if (create_directory(asset_dir) == -1) {
        delete_all(a_dir);
        return NOK;
    }

    char *start_fulltime = get_seconds_elapsed();
    char *date_time = get_date();

    char start_file[70];
    char buffer[140];

    sprintf(buffer, "%s %s %s %s %s %s %s", uid, auction_name, file_name, start_value, time_active, date_time, start_fulltime);

    //printf("buffer: %s\n", buffer);
    free(start_fulltime);
    free(date_time);

    sprintf(start_file, "%sSTART_%s.txt", a_dir, aid);
    
    if (create_file(start_file, buffer) == -1) {
        delete_all(a_dir);
        return NOK;
    }
    char auction_path[70];
    sprintf(auction_path, "%s%s.txt", hosted_dir, aid);

    if (create_file(auction_path, "") == -1){
        delete_all(a_dir);
        return NOK;
    }   
    return OK;

}

int close_auction(char uid[10], char password[10], char aid[5]){
    char user_directory[30];
    char login_file[50];
    char pass_file[50];
    char hosted_dir[50];
    char hosted_auction[70];
    char a_dir[25];

    sprintf(user_directory, "./AS/USERS/%s/", uid);
    sprintf(login_file, "%s%s_login.txt", user_directory, uid);
    sprintf(pass_file, "%s%s_pass.txt", user_directory, uid);
    sprintf(hosted_dir, "%sHOSTED/", user_directory);
    sprintf(hosted_auction, "%s%s.txt", hosted_dir, aid);
    sprintf(a_dir, "./AS/AUCTIONS/%s/", aid);

    //check login------------------------------------------------
    if (!directory_exists(user_directory)) {
        printf("User not registered.\n");
        return NLG;
    }
    else if (!file_exists(login_file)) {
        printf("User not logged in.\n");
        return NLG;
    }
    else if (!file_exists(pass_file)) {
        printf("User is not registered.\n");
        return NLG;
    }
    else if(is_password_correct(pass_file, password) == 0) {
        return NOK;
    }
    if (!folder_exists(a_dir)) {
        printf("Auction does not exist.\n");
        return EAU;
    } else if (!file_exists(hosted_auction)) {
        printf("User is not hosting this auction.\n");
        return EOW;
    } 
    if (!is_auction_active(aid)) { //closes auction if duration has finished
        return END;
    } else {
        char buffer[140] , end_file[50];
        memset(buffer, '\0', sizeof(buffer));
        char *date = get_date();
        char *secs = get_seconds_elapsed();
        sprintf(buffer, "%s %s", date, secs);
        sprintf(end_file, "%sEND_%s.txt", a_dir, aid);
        free(date);
        free(secs);


        if (create_file(end_file, buffer) == -1) {
            printf("Error creating end file");
            return NOK;
        }
        return OK;
    }
}

int create_bid(char uid[10], char password[10], char aid[5], char bid_value[10]){
    char user_directory[30];
    char login_file[50];
    char pass_file[50];
    char hosted_dir[50];
    char hosted_auction[70];
    char user_bidded_file[70];
    char a_dir[25];
    char bids_dir[60];
    char bid_file[70];
    char buffer[140];
    char *date = get_date();
    char *secs = get_seconds_elapsed();

    int v = atoi(bid_value);
    int a = atoi(aid);

    sprintf(user_directory, "./AS/USERS/%s/", uid);
    sprintf(login_file, "%s%s_login.txt", user_directory, uid);
    sprintf(pass_file, "%s%s_pass.txt", user_directory, uid);
    sprintf(hosted_dir, "%sHOSTED/", user_directory);
    sprintf(hosted_auction, "%s%s.txt", hosted_dir, aid);
    sprintf(a_dir, "./AS/AUCTIONS/%s/", aid);
    sprintf(bids_dir, "%sBIDS/", a_dir);
    sprintf(bid_file, "%s%06d.txt", bids_dir, v);
    sprintf(user_bidded_file, "%sBIDDED/%03d.txt", user_directory, a);

    check_validity(aid);  //closes auction if duration has finished

    //check login------------------------------------------------
    if (!directory_exists(user_directory)) {
        printf("User not registered.\n");
        return NLG;
    }
    else if (!file_exists(login_file)) {
        printf("User not logged in.\n");
        return NLG;
    }
    else if (!file_exists(pass_file)) {
        printf("User is not registered.\n");
        return NLG;
    }
    else if(!is_password_correct(pass_file, password)) {
        return NOK;
    }
    if (!folder_exists(a_dir)) {
        printf("Auction does not exist.\n");
        return NOK;
    } else if (file_exists(hosted_auction)) {
        printf("User is hosting this auction.\n");
        return ILG;
    } 
    if (!is_auction_active(aid)) { //closes auction if duration has finished
        return NOK;
    }  
    else if (!is_bid_greater(aid, bid_value)) {
        return REF;
    }
    else {
        memset(buffer, '\0', sizeof(buffer));
        sprintf(buffer, "%s %s %s %s", uid, bid_value, date, secs);
        free(date);
        free(secs);

        if (file_exists(bid_file)) {
            printf("bid file already exists\n");
        }


        printf("%s\n", bid_file);

        if (create_file(bid_file, buffer) == -1) {
            printf("Error creating bid file");
            return NOK;
        }

        if (!file_exists(user_bidded_file)) {
            if (create_file(user_bidded_file, "") == -1) {
                printf("Error creating bid file");
                return NOK;
            }
        }
        //falta criar ficheiro na pasta do user bidded
        return ACC;  //por ACC
    }
}

/*
int main() {
    char username[10] = "234234";
    char password[10] = "23423423";
    
    //clear_directory("./AS/USERS/");
    //clear_directory("./AS/AUCTIONS/");
    
    //printf("%d\n", create_bid("123123", "12312312", "001", "999999"));

    auction a;

     // SHOW RECORD
    printf("res: %d\n", get_record("001",&a));

    printf("I: %s %s %s %s %s %s %s %s\n", a.host_uid, a.auction_name, a.asset_name, a.start_value, a.time_active, a.start_date, a.start_time, a.start_fulltime);

    int i = 0;

    while (a.bids[i].last_bid != last)
    {   

        printf("B %s %s %s %s %s\n", a.bids[i].bidder_UID, a.bids[i].bid_value, a.bids[i].bid_date, a.bids[i].bid_time, a.bids[i].bid_sec_time);
        i++;
    }
    if (a.active == 0) {
        printf("E %s %s %s\n", a.end_date, a.end_time, a.end_sec_time);
    }


    char *s= get_asset_name("004");
    printf("%s\n", s);
    free(s);
    */

    //delete_all("./AS/USERS/345345/");
    //printf("create: %d\n", create_auction("345345", "34534534", "auchan", "100", "15", "file.txt"));
    //printf("%d\n", close_auction("345345", "34534534", "006"));

    /*
    auction a;

     // SHOW RECORD
    printf("res: %d\n", get_record("002",&a));

    printf("I: %s %s %s %s %s %s %s %s\n", a.host_uid, a.auction_name, a.asset_name, a.start_value, a.time_active, a.start_date, a.start_time, a.start_fulltime);

    int i = 0;

    while (a.bids[i].last_bid != last)
    {   

        printf("B %s %s %s %s %d\n", a.bids[i].bidder_UID, a.bids[i].bid_value, a.bids[i].bid_date, a.bids[i].bid_time, a.bids[i].last_bid);
        i++;
    }
    if (a.active == 0) {
        printf("E %s %s %s\n", a.end_date, a.end_time, a.end_sec_time);
    }
    */
   

    /*
    
    auction a[100];
    printf("%d\n", get_all_auctions(a));

    int i = 0;
    while (a[i].active != last)
    {   
        printf("%s %d\n", a[i].id, a[i].active);
        i++;
    }
    
    char *s = get_user_auctions("234234");  // corrigir!
    printf("Auctions: %s\n", s);
    free(s);

    s = 
    printf("Auctions: %s\n", s);
    if (s == NULL) {
        printf("0 auctions.\n");
    }


    //sleep(1);
    
    printf("logging out\n");
    printf("%d\n", logout("234234", "23423423"));
    printf("%d\n", unregister("234234", "23423423"));

    return 0;
}
*/