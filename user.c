#include "funcoes_udp.c"
#include <sys/stat.h>
#include <fcntl.h>

#define TCP_MSG_CHUNK 128
#define OPEN 5
#define CLOSE 6
#define SHOW_ASSET 7
#define MAKE_BID 8

int open_(char username[7], char password[9], char name[20], char start_value[20], char duration[20], char file_name[20], char ASIP[16], char ASport[6]);
int communicate_tcp(int type, char *message, size_t message_length, char ASIP[16], char ASport[6]);
int close_(char username[7], char password[9], char aid[4], char ASIP[16], char ASport[6]);
int show_asset(char aid[4], char ASIP[16], char ASport[6]);
int make_bid(char username[7], char password[9], char aid[4], char value[20], char ASIP[16], char ASport[6]);


int main (int argc, char* argv[]) {
    
    int n;
    char buffer[128], username[7],password[9];
    char function[13], arg1[20], arg2[20], arg3[20], arg4[20];

    char ASIP[16]; // n tenho a certeza se 16 é o suficiente
    char ASport[6];

    //set username as row of \0
    memset(username, '\0', sizeof(username));
    memset(password, '\0', sizeof(password));

    //input processing
    if (argc == 1) {
        strcpy(ASIP, "localhost");
        strcpy(ASport, PORT);
    }
    else if (argc == 3) {
        if (strcmp(argv[1], "-n") == 0) {
            strcpy(ASIP, argv[2]);
            strcpy(ASport, PORT);
        }
        else if (strcmp(argv[1], "-p") == 0) {
            strcpy(ASIP, "localhost");
            strcpy(ASport, argv[2]);
        }
        else {
            printf("Invalid arguments\n");
            exit(1);
        }
    }
    else if (argc == 5) {
        if (strcmp(argv[1], "-n") == 0 && strcmp(argv[3], "-p") == 0) {
            strcpy(ASIP, argv[2]);
            strcpy(ASport, argv[4]);
        }
        else if (strcmp(argv[1], "-p") == 0 && strcmp(argv[3], "-n") == 0) {
            strcpy(ASIP, argv[4]);
            strcpy(ASport, argv[2]);
        }
        else {
            printf("Invalid arguments\n");
            exit(1);
        }
    }
    else {
        printf("Invalid arguments\n");
        exit(1);
    }


    //user input
    while (1) {

        memset(function, '\0', sizeof(function));
        memset(arg1, '\0', sizeof(arg1));
        memset(arg2, '\0', sizeof(arg2));
        memset(arg3, '\0', sizeof(arg3));
        memset(arg4, '\0', sizeof(arg4));
        memset(buffer, '\0', sizeof(buffer));

        fgets(buffer, 128, stdin);
        
        n = sscanf(buffer, "%s %s %s %s %s", function, arg1, arg2, arg3, arg4);
        //printf("function: %s, arg1: %s, arg2: %s\n", function, arg1, arg2);

        //ignore just to avoid warning
        if (n == 0) {}

        if (strcmp(function, "exit") == 0) {
            exit_program(username);
        }
        else if (strcmp(function, "login") == 0) {
            if (username[0] != '\0') {
                printf("You are already logged in. Stop.\n");
                continue;
            }

            if (login(arg1, arg2, ASIP, ASport) == 1){
                strncpy(username, arg1, 6);
                strncpy(password, arg2, 8);
            }

        }
        else if (strcmp(function, "logout") == 0) {
            if (username[0] == '\0') {
                printf("You are not logged in. Stop.\n");
                continue;
            }
            if (logout(username, password, ASIP, ASport) ==-1){
                printf("Error logging out\n");
            }
            else {
                memset(username, '\0', sizeof(username));
                memset(password, '\0', sizeof(password));
            }

        }
        else if (strcmp(function, "unregister") == 0) {
            if (username[0] == '\0') {
                printf("You are not logged in. Stop.\n");
                continue;
            }
            if (unregister(username, password, ASIP, ASport) ==-1){
                printf("Error unregistering\n");
            }
            else {
                memset(username, '\0', sizeof(username));
                memset(password, '\0', sizeof(password));
            }
        }
        else if (strcmp(function, "open") == 0) {
            if (username[0] == '\0') {
                printf("You are not logged in. Stop.\n");
                continue;
            }
            else if (n != 5){
                printf("Invalid arguments\n");
                continue;
            }
            else if (open_(username, password, arg1, arg2, arg3, arg4, ASIP, ASport) == -1) {
                printf("Error opening auction\n");
                continue;
            }
        }
        else if (strcmp(function, "close") == 0) {
            if (username[0] == '\0') {
                printf("You are not logged in. Stop.\n");
                continue;
            }
            else if (n != 2){
                printf("Invalid arguments\n");
                continue;
            }
            else if (close_(username, password, arg1, ASIP, ASport) == -1) {
                printf("Error closing auction\n");
                continue;
            }
        }
        else if (strcmp(function, "list") == 0 || strcmp(function,"l") == 0) {
            if(list_(ASIP, ASport) == -1) {
                printf("Error getting list\n");
                continue;
            }
        }
        else if (strcmp(function, "myauctions") == 0 || strcmp(function,"ma") == 0) {
            if (username[0] == '\0') {
                printf("You are not logged in. Stop.\n");
                continue;
            }
            if (myauctions(username, ASIP, ASport) == -1) {
                printf("Error getting auctions\n");
                continue;
            }
        }
        else if (strcmp(function, "mybids") == 0 || strcmp(function,"mb") == 0) {
            if (username[0] == '\0') {
                printf("You are not logged in. Stop.\n");
                continue;
            }
            if (mybids(username, ASIP, ASport) == -1) {
                printf("Error getting auctions\n");
                continue;
            }
        }
        else if (strcmp(function, "show_asset") == 0 || strcmp(function,"sa") == 0) {
            if (n == 2){
                if (show_asset(arg1, ASIP, ASport) == -1) {
                    printf("Error showing asset\n");
                    continue;
                }
            }
            else
                printf("Invalid format\n");
            //show_asset(token);
        }
        else if (strcmp(function, "bid") == 0 || strcmp(function,"b") == 0) {
            if (username[0] == '\0') {
                printf("You are not logged in. Stop.\n");
                continue;
            }
            if (make_bid(username, password, arg1, arg2, ASIP, ASport)==-1) {
                printf("Error making bid\n");
                continue;
            }
        }
        else if ((strcmp(function, "show_record") == 0 || strcmp(function,"sr") == 0)) {
            if (n == 2){
                if (show_record(arg1, ASIP, ASport) == -1) {
                    printf("Error showing record of auction\n");
                    continue;
                }
            }
            else
                printf("You must specify an Auction ID\n");
        }
        else {
            printf("Invalid command\n");
        }
    }

    return 0;

}
int open_(char username[7], char password[9], char name[20], char file_name[20], char start_value[20], char duration[20], char ASIP[16], char ASport[6]) {
    printf("%s %s %s %s %s %s\n", name, file_name, start_value, duration, ASIP, ASport);

    if (strlen(name) > 10) {
            printf("Auction name must have no more than 10 characters.\n");
            return -1;
        }
    else {
            for (int i = 0; i < 10; i++) {
                if (!isalnum(name[i])) {
                    printf("Invalid auction name. Must contain only alphanumeric characters.\n");
                    return -1;
                }
            } 
        }
    if (strlen(start_value) > 6) {
            printf("Start value must have no more than 6 digits.\n");
            return -1;
        }
    else {
            for (int i = 0; i < 6; i++) {
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
            for (int i = 0; i < 5; i++) {
                if (!isalnum(name[i])) {
                    printf("Invalid auction duaration. Must contain only digits.\n");
                    return -1;
                }
            } 
        }
    
    FILE *file = fopen(file_name, "rb");  // Open the file in binary mode
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    // Get the size of the file
    struct stat st;
    size_t size;
    if (stat(file_name, &st) != 0) {
        printf("Error getting file size\n");
        fclose(file);
        return -1;
    }
    size = st.st_size;
    printf("The size of the file is %ld bytes.\n", size);

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

    char *message = (char *) malloc((size+100)*sizeof(char));
    if (message == NULL) {
        perror("Error allocating memory");
        free(file_content);
        fclose(file);
        return -1;
    }

    memset(message, '\0', (size+100));

    sprintf(message, "OPA %s %s %s %s %s %s %ld ", username, password, name, start_value, duration, file_name, size);
    size_t intro_size = strlen(message);
    sprintf(message + intro_size, "%s\n", file_content);

    free(file_content);
    printf("message: %s\n", message);

    int n = communicate_tcp(OPEN, message, intro_size +size, ASIP, ASport);
    if (n == -1) {
        printf("Error communicating with AS\n");
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
        printf("Error communicating with AS\n");
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
        printf("Error communicating with AS\n");
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
        printf("Error communicating with AS\n");
        return -1;
    }
    
    return 1;
}

int communicate_tcp(int type, char *message, size_t message_length, char ASIP[16], char ASport[6]) {
    int fd, errcode;
    ssize_t n;
    struct addrinfo hints, *res;
    size_t buffer_size = 128;
    char *buffer = (char *) malloc(buffer_size*sizeof(char));
    
    memset(buffer, '\0',buffer_size);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        printf("Error creating socket\n");
        return -1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    if (ASIP==NULL || ASport==NULL){ //ignore just to avoid warning
    }
    //DEPOIS ALTERAR IP E PORT PARA ASIP E ASport !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    errcode = getaddrinfo(TEJO, "58011", &hints, &res);
    if(errcode != 0){
        printf("Error getting address info\n");
        return -1;
    }

    n = connect(fd, res->ai_addr, res->ai_addrlen);
    if(n == -1){
        printf("Error connecting\n");
        return -1;
    }


   size_t total_sent = 0;

   while (total_sent < message_length) {
        size_t remaining = message_length - total_sent;
        size_t to_send = remaining > TCP_MSG_CHUNK ? TCP_MSG_CHUNK : remaining;

        n = write(fd, message + total_sent, to_send);
        printf("%.*s", n, message + total_sent);
        if (n == -1) {
            printf("Error writing\n");
            return -1;
        }

        total_sent += n;
    }
    printf("total_sent: %ld\n", total_sent);

    size_t total_read = 0;

    n = read(fd, buffer, 128);  
    if(n == -1){
        printf("Error reading\n");
        return -1;
    }
    
    while (1) {
        ssize_t n = read(fd, buffer + total_read, buffer_size - total_read);
        if (n == -1) {
            printf("Error reading\n");
            free(buffer);
            return -1;
        } else if (n == 0) { // End of file, no more data to read
            break;
        }

        total_read += n;

        if (total_read >= buffer_size) { // Buffer full, need to expand
            buffer_size *= 2;
            char *temp_buffer = (char *) realloc(buffer, buffer_size * sizeof(char));
            if (temp_buffer == NULL) {
                printf("Memory reallocation failed\n");
                free(buffer);
                return -1;
            }
            buffer = temp_buffer;
        } 
    }
    printf("buffer: %s\n", buffer);

    freeaddrinfo(res);
    close(fd);

    switch (type)
    {
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
                    printf("Received Impossible AID\n");
                    return -1;
                }
            }
            printf("Auction opened with ID: %s\n", AID);
            return 1;
        }
        else if (strcmp(buffer, "ROA NOK\n") == 0){
            printf("Error opening auction\n");
            return -1;
        }
        else if (strcmp(buffer, "ROA NLG\n") == 0)
        {
            printf("User not logged in\n");
            return -1;
        }
        else {
            printf("Error receiving answer from AS\n");
            return -1;
        }
        break;
    }
    case CLOSE: {
        char arg1[5], arg2[5];
        memset(arg1, '\0', sizeof(arg1));
        memset(arg2, '\0', sizeof(arg2));

        int j = sscanf(buffer, "%s %s", arg1,arg2);
        if (strcmp(arg1, "RCL") == 0 && strcmp(arg2, "OK") == 0 && j==2){
            printf("Auction closed\n");
            return 1;
        }
        else if (strcmp(buffer, "RCL NOK\n") == 0){
            printf("Error closing auction\n");
            return -1;
        }
        else if (strcmp(buffer, "RCL NLG\n") == 0)
        {
            printf("User not logged in\n");
            return -1;
        }
        else if (strcmp(buffer, "RCL EAU\n") == 0)
        {
            printf("Auction does not exist\n");
            return -1;
        }
        else if (strcmp(buffer, "RCL EOW\n") == 0)
        {
            printf("You are not the owner of this auction. Stop.\n");
            return -1;
        }
        else if (strcmp(buffer, "RCL END\n") == 0)
        {
            printf("Auction has already finished\n");
            return -1;
        }
        else {
            printf("Invalid AID\n");
            return -1;
        }
        break;
    }
    case MAKE_BID: {
        if (strcmp(buffer, "RBD ACC\n") == 0) {
            printf("Bid made\n");
            return 1;
        }
        else if (strcmp(buffer, "RBD NOK\n") == 0) {
            printf("Auction not active\n");
            return -1;
        }
        else if (strcmp(buffer, "RBD NLG\n") == 0) {
            printf("User not logged in\n");
            return -1;
        }
        else if (strcmp(buffer, "RBD REF\n") == 0) {
            printf("Your bid must be greater than current bid\n");
            return -1;
        }
        else if (strcmp(buffer, "RBD ILG\n") == 0) {
            printf("You can't bid on your own auction. Stop.\n");
            return -1;
        }
        else if (strcmp(buffer, "RBD EBD\n") == 0) {
            printf("Bid is lower than the current highest bid\n");
            return -1;
        }
        else {
            printf("Invalid format\n");
            return -1;
        }
        break;
    }
    case SHOW_ASSET: {
        char arg1[5], arg2[5], Fname[26], Fsize[10];
        memset(arg1, '\0', sizeof(arg1));
        memset(arg2, '\0', sizeof(arg2));
        memset(Fname, '\0', sizeof(Fname));
        memset(Fsize, '\0', sizeof(Fsize));

        int j = sscanf(buffer, "%s %s %s %s", arg1,arg2, Fname, Fsize);
        int Fbytes = atoi(Fsize);
        
        char *Fdata = (char *) malloc(Fbytes*sizeof(char));  //falta igualar fdata a alguma coisa!
        if (Fdata == NULL) {
            perror("Error allocating memory");
            return -1;
        }

        if(strcmp(arg1, "RSA") == 0 && strcmp(arg2, "OK") == 0 && j==4){
            
            // TRABALHA MALANDRO
            // SE É ASSIM TÃO FACIL GUARDAR O FICHEIRO NO DIRETORIO LOCAL,
            // TENS UMA OTIMA OPORTUNIDADE DE MOSTRAR QUE EU SOU INCOMPETENTE
            // ENQUANTO ISSO, EU NÃO TOU A FRITAR A CAROLA :)))))))

            //por freesss
            char path[40];
            memset(path, '\0', sizeof(path));
            snprintf(path, sizeof(path), "./files/%s", Fname);
            FILE *file = fopen(Fname, "w");  // Open the file in binary mode
            if (file == NULL) {
                perror("Error opening file");
                free(buffer);
                free(Fdata);
                return -1;
            }

            // Write the file content
            int read_size = (int) fwrite(Fdata, 1, Fbytes, file);
            if (read_size != Fbytes) {
                perror("Error reading file");
                free(buffer);
                free(Fdata);
                fclose(file);
                return -1;
            }

            free(buffer);
            free(Fdata);
            return 1;
        }
        else if (strcmp(buffer, "RSA NOK\n") == 0){
            printf("Error showing asset\n");
            free(buffer);
            free(Fdata);
            return -1;
        }
        else {
            printf("Unexpected protocol message\n");
            free(buffer);
            free(Fdata);
            return -1;
        }
    }
    default:
        printf("Error communicating with AS\n");
        break;
    }

    return 1;
}

