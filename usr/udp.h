#define MAX_UDP_RCVSIZE 10001

#define LOGIN 0
#define LOGOUT 1
#define UNREGISTER 2
#define LIST 3
#define SHOW_RECORD 4

int login(char username[20], char pass[20], char ASIP[16], char ASport[6]);
int logout(char username[7], char password[9], char ASIP[16], char ASport[6]);
int unregister(char username[7], char password[9], char ASIP[16], char ASport[6]);
int list_(char ASIP[16], char ASport[6]);
int myauctions(char username[7], char ASIP[16], char ASport[6]);
int mybids(char username[7], char ASIP[16], char ASport[6]);
int show_record(char AID[7], char ASIP[16], char ASport[6]);
void exit_program(char username[6]);
int communicate_udp(int type, char message[25], char ASIP[16], char ASport[6]);