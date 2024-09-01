#include <iostream>
#include "User.hpp"

using namespace std;

int main() {
    User user1("Hidaya Mustafa", 1);
    cout << "---> User: " << user1.getName() << ", ID: " << user1.getId() << "\n";
   
     User user2("Ahmad",2);
    user2.addTransaction(2,TransactionType::DEPOSIT ,Categories::NONE, 200.0);
    User user3("Raneem",3);
    User user4("Saja",4);

    user1.setBudget(Categories::FOOD, 300);
    user1.setBudget(Categories::SHOES, 300);
    user2.setBudget(Categories::FOOD, 300);
    
    user1.addTransaction(0, TransactionType::DEPOSIT ,Categories::NONE, 300.0);
    user1.addTransaction(1, TransactionType::WITHDRAW, Categories::FOOD, 200.0);

    user1.setSavingPlan(0, 200.0, Date(15, 8, 2024), Date(20, 8, 2024));
    
    user1.getSavingPlan(0)->transferAmount(100);
    user1.getSavingPlan(0)->checkSavingPlan();

    user1.updateSavingPlan(0, 100.0, Date(15, 8, 2024), Date(20, 8, 2024));
    user1.getSavingPlan(0)-> checkSavingPlan();

    user1.setSavingPlan(1, 250.0, Date(11, 8, 2024), Date(20, 8, 2024));

    user1.deleteSavingPlan(0);

    user1.addTransaction(2,TransactionType::DEPOSIT ,Categories::NONE, 500.0);

    user1.generateReport(Date(11, 8, 2024), Date(21, 8, 2024));


    SharedTransaction sh(101, 500.0, Categories::FOOD, user1,250.0); 
    sh.addParticipant(user2,400,150);
    //sh.addParticipant(user3,100,150);

    sh.printParticipants();
    sh.updateParticipantPaid(user1,200);

    //SharedTransaction sh2(102, 500.0, Categories::FOOD, user1, 250.0); 
    //sh2.addParticipant(user4,100,150);

    std::cout<<"//////////////////////////\n";
    user2.printSharedTransaction();
    user1.printSharedTransaction();

    sh.exitParticipant(user1);

    sh.printParticipants();
    //sh2.printParticipants();
    user1.printSharedTransaction();

    return 0;
}
