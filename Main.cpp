#include <iostream>
#include "User.hpp"

using namespace std;

int main() {
    DataBaseManager& db = DataBaseManager::getInstance();
    User user1("Hidaya Mustafa");
    User user2("Ahmad");
    User user3("Abrar");
    User user4("Omar");
    User user5("Eman");
    

    user1.setBudget(Categories::FOOD, 300);
    user1.setBudget(Categories::SHOES, 300);
    user2.setBudget(Categories::FOOD, 300);
    user3.setBudget(Categories::FOOD, 300);

    db.printUsers();

    user1.setSavingPlan(550,Date(1,9,2024),Date(10,10,2024));
    user3.setSavingPlan(3000,Date(15,9,2024),Date(15,12,2024));

    db.printSavingPlans();
    user3.addSavingAmount(2,200.4);
    user3.addSavingAmount(2,10);
    user3.updateSavingPlan(2,1000,Date(15,9,2024),Date(15,12,2024));
    user1.deleteSavingPlan(1);

    db.printSavingPlans();

    user1.addTransaction(TransactionType::DEPOSIT ,Categories::NONE, 300.0);
    user1.addTransaction(TransactionType::DEPOSIT ,Categories::NONE, 500.0);
    user2.addTransaction(TransactionType::DEPOSIT ,Categories::NONE, 200.0);
    user3.addTransaction(TransactionType::DEPOSIT ,Categories::NONE, 200.0);
    

    SharedTransaction *sh = new SharedTransaction(600.0, Categories::FOOD, user1);
    sh->addParticipant(user2,150);
    db.printTransactions();
    db.printParticipants();


    return 0;
}
