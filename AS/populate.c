#include "db.c"

int main(){
    int ret;
    
    login_user("234234", "23423423");
    login_user("123123", "12312312");

    create_auction("123123", "12312312", "Auction_name", "10", "3600", "tarzan.txt");
    create_auction("123123", "12312312", "other_name", "10", "60", "diamond.txt");


    
    return 0;
}