#include "db.c"

int main(){
    int ret;
    
    login_user("234234", "23423423");
    login_user("123123", "12312312");

    //create_auction("123123", "12312312", "Auction_name", "10", "3600", "tarzan.txt");
    //create_auction("123123", "12312312", "other_name", "10", "1300", "diamond.txt");

    int n= create_bid("234234", "23423423", "001", "15");
    printf("n=%d\n", n);
    create_bid("234234", "23423423", "002", "15");
    create_bid("345345", "34534534", "002", "20");

    
    return 0;
}