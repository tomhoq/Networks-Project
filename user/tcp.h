#define TCP_MSG_CHUNK 128

#define OPEN 5
#define CLOSE 6
#define SHOW_ASSET 7
#define MAKE_BID 8
#define PORT "58025"
#define TEJO "tejo.tecnico.ulisboa.pt"

int open_(char username[7], char password[9], char name[20], char start_value[20], char duration[20], char file_name[20], char ASIP[16], char ASport[6]);
int communicate_tcp(int type, char *message, size_t message_length, char ASIP[16], char ASport[6]);
int close_(char username[7], char password[9], char aid[4], char ASIP[16], char ASport[6]);
int show_asset(char aid[4], char ASIP[16], char ASport[6]);
int make_bid(char username[7], char password[9], char aid[4], char value[20], char ASIP[16], char ASport[6]);