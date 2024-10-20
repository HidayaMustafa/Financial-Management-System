#include <iostream>
#include "User.hpp"

using namespace std;

int main() {
    User user1("Hidaya Mustafa", 1);
    
    user1.setBudget(Categories::FOOD, 300);
    user1.setBudget(Categories::SHOES, 300);
    
    user1.addTransaction(0, TransactionType::DEPOSIT ,Categories::NONE, 300.0);
    user1.addTransaction(1, TransactionType::WITHDRAW, Categories::FOOD, 200.0);
    user1.addTransaction(2,TransactionType::DEPOSIT ,Categories::NONE, 500.0);

    
    user1.generateReport(Date(11, 8, 2024), Date(3, 9, 2024));

    SharedTransaction *sh = new SharedTransaction(101, 300.0, Categories::FOOD, user1)
    sh->addParticipant(user2,150);
   
    user1.generateReport(Date(11, 8, 2024), Date(3, 9, 2024));
   
    return 0;
}
