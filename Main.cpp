#include <iostream>
#include "User.hpp"

using namespace std;

int main() {
    User user1("Hidaya Mustafa", 1);
    //cout << "---> User: " << user1.getName() << ", ID: " << user1.getId() << "\n";
      
    User user2("Ahmad",2);
    user2.addTransaction(7,TransactionType::DEPOSIT ,Categories::NONE, 200.0);
    
    User user3("Abrar",3);
    user3.addTransaction(8,TransactionType::DEPOSIT ,Categories::NONE, 200.0);
    
    user1.setBudget(Categories::FOOD, 300);
    user1.setBudget(Categories::SHOES, 300);
    user2.setBudget(Categories::FOOD, 300);
    user3.setBudget(Categories::FOOD, 300);
    
    user1.addTransaction(0, TransactionType::DEPOSIT ,Categories::NONE, 300.0);
    //user1.addTransaction(1, TransactionType::WITHDRAW, Categories::FOOD, 200.0);
    user1.addTransaction(2,TransactionType::DEPOSIT ,Categories::NONE, 500.0);
    cout << "---> User: " << user1.getName() << ", ID: " << user1.getId() << "\n";
    user1.generateReport(Date(11, 8, 2024), Date(3, 9, 2024));

    SharedTransaction sh(101, 300.0, Categories::FOOD, user1); 

    sh.printParticipants();
    sh.addParticipant(user2,150);
    cout <<"\n\n";
    sh.printParticipants();
    cout <<"\n\n";
    cout << "---> User: " << user1.getName() << ", ID: " << user1.getId() << "\n";
    user1.generateReport(Date(11, 8, 2024), Date(3, 9, 2024));
    sh.addParticipant(user3,50);
    cout <<"\n\n";
    sh.printParticipants();
    cout <<"\n\n";
    cout << "---> User: " << user1.getName() << ", ID: " << user1.getId() << "\n";
    user1.generateReport(Date(11, 8, 2024), Date(3, 9, 2024));
/*
    sh.updateParticipantPaid(user1,100);

    user2.printSharedTransaction();
    user1.printSharedTransaction();

    sh.exitParticipant(user1);

    user1.printSharedTransaction();*/

    return 0;
}
