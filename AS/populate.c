#include "db.c"

int main(){
    int ret;
    
    login_user("234234", "23423423");
    login_user("123123", "12312312");
    char str[10];  // Adjust the size based on your needs

    // Using sprintf to convert integer to string

    create_auction("123123", "12312312", "Auction_name", "10", "3600", "tarzan.txt");
    //create_auction("123123", "12312312", "other_name", "10", "1300", "diamond.txt");

    //int n= create_bid("234234", "23423423", "001", "15");
    //create_bid("345345", "34534534", "001", "20");
    for (int i = 1; i < 100; i++) {
        sprintf(str, "%d", i+ 10);
        create_bid("234234", "23423423", "002", str);
    }


    
    return 0;
}