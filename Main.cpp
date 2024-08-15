#include <iostream>
#include "User.cpp"

using namespace std;


int main() {
    User user1("Hidaya Mustafa", 10);
    cout << "---> User: " << user1.getName() << ", ID: " << user1.getId() << "\n";
    user1.getWallet().setBudget(Food,150);
    user1.getWallet().setBudget(Shoes,300);
    user1.getWallet().addTransaction(0,Deposit,null,300.0);
    user1.getWallet().addTransaction(1,Withdraw,Food,100.0);
    user1.getWallet().addTransaction(2,Withdraw,Food,30.0);
    //user1.getWallet().updateTransaction(1,Deposit,null,200.0);
    user1.getWallet().addTransaction(2,Withdraw,Shoes,100.0);
    user1.getWallet().updateTransaction(0,Withdraw,Shoes,50.0);
    user1.getWallet().deleteTransaction(0);
    user1.getWallet().generateReport(Date(11,8,2024) , Date(13,8,2024));

    return 0;
}