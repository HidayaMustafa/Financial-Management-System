#include <iostream>
#include "User.hpp"

using namespace std;

int main() {
    User user1("Hidaya Mustafa", 1);
    cout << "---> User: " << user1.getName() << ", ID: " << user1.getId() << "\n";
   
    user1.setBudget(Categories::FOOD, 300);
    user1.setBudget(Categories::SHOES, 300);
    
    user1.addTransaction(0, TransactionType::DEPOSIT ,Categories::NONE, 300.0);
    user1.addTransaction(1, TransactionType::WITHDRAW, Categories::FOOD, 200.0);

    user1.setSavingPlan(0, 200.0, Date(15, 8, 2024), Date(20, 8, 2024));
    
    user1.getSavingPlan(0)->transferAmount(100);
    user1.getSavingPlan(0)->checkSavingPlan();

    user1.updateSavingPlan(0, 100.0, Date(15, 8, 2024), Date(20, 8, 2024));
    user1.getSavingPlan(0)-> checkSavingPlan();

    user1.setSavingPlan(1, 250.0, Date(11, 8, 2024), Date(20, 8, 2024));

    user1.deleteSavingPlan(0);

    user1.addTransaction(2,TransactionType::DEPOSIT ,Categories::NONE, 200.0);

    user1.generateReport(Date(11, 8, 2024), Date(21, 8, 2024));

    User user2("Ahmad",2);

    SharedTransaction& sh1 = user1.createSharedTransaction(3,500,Categories::FOOD,150);
    sh1.addParticipant(user2,100,150);

    sh1.printParticipants();

    SharedTransaction& sh2 = user1.createSharedTransaction(4,500,Categories::FOOD,150);

    user1.printSh();

    return 0;
}
