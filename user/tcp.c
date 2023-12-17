#include "tcp.h"
#include <unistd.h>

int open_(char username[7], char password[9], char name[20], char file_name[30], char start_value[20], char duration[20], char ASIP[16], char ASport[6]) {
    
    if (strlen(name) > 10) {
            printf("Auction name must have no more than 10 characters.\n");
            return -1;
    }
    else {
        for (int i = 0; i < (int)strlen(name); i++) {
            if (!isalnum(name[i])&& file_name[i] != '-' && file_name[i] != '_' && file_name[i] != '.') {
                printf("Invalid auction name. Must contain only alphanumeric characters.\n");
                return -1;
            }
        } 
    }
    if (strlen(file_name) > 24) {
        printf("File name must have no more than 24 characters.\n");
        return -1;
    }
    else {
        for (int i = 0; i < (int)strlen(file_name); i++) {
            if (!isalnum(file_name[i]) && file_name[i] != '-' && file_name[i] != '_' && file_name[i] != '.') {
                printf("Invalid file name.\n");
                return -1;
            }
        } 
    }
    if (strlen(start_value) > 6) {
            printf("Start value must have no more than 6 digits.\n");
            return -1;
    }
    else {
            for (int i = 0; i < (int)strlen(start_value); i++) {
                if (!isdigit(start_value[i])) {
                    printf("Invalid start time. Must contain only digits.\n");
                    return -1;
                }
            } 
    }
    if (strlen(duration) > 5) {
            printf("Auction duration must have no more than 5 digits.\n");
            return -1;
    }
    else {
        for (int i = 0; i < (int)strlen(duration); i++) {
            if (!isdigit(duration[i])) {
                printf("Invalid auction duration. Must contain only digits.\n");
                return -1;
            }
        } 
    }

    char path_file[50];
    memset(path_file, '\0', sizeof(path_file));
    sprintf(path_file, "./user/test/%s", file_name);
    FILE *file = fopen(path_file, "rb"); // Open the file in binary mode
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    // Get the size of the file
    struct stat st;
    size_t size;
    if (stat(path_file, &st) != 0) {
        printf("Error getting file size.\n");
        fclose(file);
        return -1;
    }
    size = st.st_size;
    if (size > 10000000) {
        printf("File size must be less than 10MB.\n");
        fclose(file);
        return -1;
    }

    // Allocate memory to store the file content
    unsigned char *file_content = malloc((size+1)); // +1 for the \0
    if (file_content == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return -1;
    }

    memset(file_content, '\0', size+1);
    // Read the file content
    size_t read_size = fread(file_content, 1, size, file);
    if (read_size != size) {
        perror("Error reading file");
        free(file_content);
        fclose(file);
        return -1;
    }

    fclose(file);

    char *message = (char *) malloc((size+100)*sizeof(char));
    if (message == NULL) {
        perror("Error allocating memory.\n");
        free(file_content);
        return -1;
    }

    memset(message, '\0', (size+100));
    sprintf(message, "OPA %s %s %s %s %s %s %ld ", username, password, name, start_value, duration, file_name, size);
    size_t intro_size = strlen(message);
    memcpy(message + intro_size, file_content, size);
    free(file_content);

    int n = communicate_tcp(OPEN, message, intro_size +size, ASIP, ASport);
    if (n == -1) {
        printf("Error communicating with AS.\n");
        free(message);
        return -1;
    }

    free(message);
    return 1;
}

int close_(char username[7], char password[9], char aid[4], char ASIP[16], char ASport[6]) {
    char message[128];
    memset(message, '\0', sizeof(message));
    sprintf(message, "CLS %s %s %s\n", username, password, aid);

    size_t message_length = strlen(message);

    int n = communicate_tcp(CLOSE, message, message_length,ASIP, ASport);
    if (n == -1) {
        printf("Error communicating with AS.\n");
        return -1;
    }
    return 1;
}

int show_asset(char aid[4], char ASIP[16], char ASport[6]) {
    char message[25];
    memset(message, '\0', sizeof(message));
    sprintf(message, "SAS %s\n", aid);

    size_t message_length = strlen(message);

    int n = communicate_tcp(SHOW_ASSET, message, message_length, ASIP, ASport);
    if (n == -1) {
        printf("Error communicating with AS.\n");
        return -1;
    }
    return 1;
}

int make_bid(char username[7], char password[9], char aid[4], char value[20], char ASIP[16], char ASport[6]) {
    char message[128];
    memset(message, '\0', sizeof(message));
    sprintf(message, "BID %s %s %s %s\n", username, password, aid, value);

    size_t message_length = strlen(message);

    int n = communicate_tcp(MAKE_BID, message, message_length, ASIP, ASport);
    if (n == -1) {
        printf("Error communicating with AS.\n");
        return -1;
    }
    return 1;
}

int communicate_tcp(int type, char *message, size_t message_length, char ASIP[16], char ASport[6]) {
    int fd, errcode;
    ssize_t n;
    struct addrinfo hints, *res;
    size_t buffer_size = 128;
    char buffer[buffer_size];
    
    memset(buffer, '\0',buffer_size);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        printf("Error creating socket.\n");
        return -1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    if (ASIP==NULL || ASport==NULL) {}
    errcode = getaddrinfo(ASIP, ASport, &hints, &res);
    if(errcode != 0){
        printf("Error getting address.\n");
        return -1;
    }

    n = connect(fd, res->ai_addr, res->ai_addrlen);
    if(n == -1){
        printf("Error in connecting.\n");
        return -1;
    }

   size_t total_sent = 0;
   while (total_sent < message_length) {
        size_t remaining = message_length - total_sent;
        size_t to_send = remaining > TCP_MSG_CHUNK ? TCP_MSG_CHUNK : remaining;

        n = write(fd, message + total_sent, to_send);
        if (n == -1) {
            printf("Error writing.\n");
            return -1;
        }

        total_sent += n;
    }

    size_t total_read = 0;
    char code[5];
    if (type != SHOW_ASSET) {
        ssize_t n = read(fd, buffer, sizeof(buffer));
        freeaddrinfo(res);
        close(fd);
    }
        

    switch (type) {
        
        // Open
        case OPEN: {

            char AID[4], arg1[5], arg2[5];
            memset(AID, '\0', sizeof(AID));
            memset(arg1, '\0', sizeof(arg1));
            memset(arg2, '\0', sizeof(arg2));

            int j = sscanf(buffer, "%s %s %s", arg1,arg2, AID);

            if (strcmp(arg1, "ROA") == 0 && strcmp(arg2, "OK") == 0 && j==3 && strlen(AID) ==3){
                for (int i = 0; i < 3; i++)
                {
                    if (AID[i] < '0' || AID[i] > '9'){
                        printf("Invalid AID.\n");
                        return -1;
                    }
                }
                printf("Auction opened with ID: %s\n", AID);
                return 1;
            }
            else if (strcmp(buffer, "ROA NOK\n") == 0) {
                printf("Error opening auction.\n");
                return 0;
            }
            else if (strcmp(buffer, "ROA NLG\n") == 0) {
                printf("You are not logged in. Stop.\n");
                return 0;
            }
            else {
                printf("Unexpected protocol message.\n");
                return -1;
            }
            break; 
        }

        // Close
        case CLOSE: {

            char arg1[5], arg2[5];
            memset(arg1, '\0', sizeof(arg1));
            memset(arg2, '\0', sizeof(arg2));

            int j = sscanf(buffer, "%s %s", arg1,arg2);

            if (strcmp(arg1, "RCL") == 0 && strcmp(arg2, "OK") == 0 && j==2){
                printf("Auction closed.\n");
                return 1;
            }
            else if (strcmp(buffer, "RCL NOK\n") == 0){
                printf("Error closing auction.\n");
                return 0;
            }
            else if (strcmp(buffer, "RCL NLG\n") == 0)
            {
                printf("You are not logged in. Stop.\n");
                return 0;
            }
            else if (strcmp(buffer, "RCL EAU\n") == 0)
            {
                printf("Auction does not exist.\n");
                return 0;
            }
            else if (strcmp(buffer, "RCL EOW\n") == 0)
            {
                printf("You are not the owner of this auction. Stop.\n");
                return 0;
            }
            else if (strcmp(buffer, "RCL END\n") == 0)
            {
                printf("Auction has already finished.\n");
                return 0;
            }
            else {
                printf("Unexpected protocol message.\n");
                return -1;
            }
            break;
        }

        // Show Asset
        case SHOW_ASSET: {

            char arg1[5], arg2[5], Fname[26], Fsize[10], arg3[120];
            memset(arg1, '\0', sizeof(arg1));
            memset(arg2, '\0', sizeof(arg2));
            memset(Fname, '\0', sizeof(Fname));
            memset(Fsize, '\0', sizeof(Fsize));
            memset(arg3, '\0', sizeof(arg3));
            char tmp[128];
            memset(tmp, '\0', sizeof(tmp));

            ssize_t ret = read(fd, tmp, 128);
            if (ret == -1) {
                printf("Error reading.\n");
                return -1;
            }
            int j = sscanf(tmp, "%s %s %s %s %[^\n]", arg1, arg2, Fname, Fsize, arg3);
            int bytes_to_read = atoi(Fsize);
            
            int size_arg3 = 128 - (strlen(arg1) + strlen(arg2) + strlen(Fname) + strlen(Fsize) + 4);
            if(strcmp(arg1, "RSA") == 0 && strcmp(arg2, "OK") == 0){
                char path[40];
                memset(path, '\0', sizeof(path));
                snprintf(path, sizeof(path), "./user/assets/%s", Fname);

                FILE *file = fopen(path, "w");  // Open the file in binary mode
                if (file == NULL) {
                    file = fopen(path, "w+"); // Create the file if it doesn't exist
                }
                if (file == NULL) {
                    perror("Error opening file.\n");
                    return -1;
                }

                // Write the file content
                int bytes_read = 0;
                int wr = (int) fwrite(arg3, 1, size_arg3, file);
                if (wr != size_arg3) {
                    perror("Error reading file.\n");
                    fclose(file);
                    return -1;
                }
                bytes_read += wr;

                while (1) {
                    memset(tmp, '\0', sizeof(tmp));
                    ret = read(fd, tmp, 127);
                    wr = (int) fwrite(tmp, 1, ret, file);
                    if (wr != ret) {
                        perror("Error reading file2\n");
                            fclose(file);
                        return -1;
                    }
                    fflush(file);
                    bytes_read += wr;
                    if (bytes_read >= bytes_to_read) {
                        break;
                    }
                }
                fclose(file);
                freeaddrinfo(res);
                close(fd);
                return 1;
            }
            else if (strcmp(arg1, "RSA") == 0 && strcmp(arg2, "NOK") == 0 && j==4) {
                printf("Error showing asset.\n");
                freeaddrinfo(res);
                close(fd);
                return 0;
            }
            else if (strcmp(arg1, "RSA") == 0 && strcmp(arg2, "ERR") == 0 && j==4) {
                printf("Auction does not exist.\n");
                freeaddrinfo(res);
                close(fd);
                return 0;
            }
            else {
                printf("Unexpected protocol message.\n");
                freeaddrinfo(res);
                close(fd);
                return -1;
            }
            break;
        }

        // Bid
        case MAKE_BID: {
            if (strcmp(buffer, "RBD ACC\n") == 0) {
                printf("Bid accepted.\n");
                return 1;
            }
            else if (strcmp(buffer, "RBD NOK\n") == 0) {
                printf("Auction not active.\n");
                return 0;
            }
            else if (strcmp(buffer, "RBD NLG\n") == 0) {
                printf("You are not logged in. Stop.\n");
                return 0;
            }
            else if (strcmp(buffer, "RBD REF\n") == 0) {
                printf("Your bid must be greater than current bid.\n");
                return 0;
            }
            else if (strcmp(buffer, "RBD ILG\n") == 0) {
                printf("You can't bid on your own auction. Stop.\n");
                return 0;
            }
            else if (strcmp(buffer, "RBD EBD\n") == 0) {
                printf("Bid is lower than the current highest bid.\n");
                return 0;
            }
            else {
                printf("Unexpected protocol message.\n");
                return -1;
            }
            break;
        }
        default:
            printf("Unexpected protocol message.\n");
            break;
    }
    return 1;
}