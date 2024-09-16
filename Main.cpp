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


    user1.setBudget(Categories::FOOD, 600);
    user1.setBudget(Categories::SHOES, 300);
    user2.setBudget(Categories::FOOD, 300);
    user3.setBudget(Categories::CLOTHES, 300);
    user3.setBudget(Categories::FOOD, 200);
    user5.setBudget(Categories::ELECTRONICS,2000);
    db.printCategoryBudgetTable();


    db.printUsers();

    user1.setSavingPlan(550,Date(1,9,2024),Date(10,10,2024));
    user3.setSavingPlan(3000,Date(15,9,2024),Date(15,12,2024));


    db.printSavingPlans();

    user1.addTransaction(TransactionType::DEPOSIT ,Categories::NONE, 300.0);
    user1.addTransaction(TransactionType::DEPOSIT ,Categories::NONE, 500.0);
    user2.addTransaction(TransactionType::DEPOSIT ,Categories::NONE, 200.0);
    user3.addTransaction(TransactionType::DEPOSIT ,Categories::NONE, 200.0);
    user3.addTransaction(TransactionType::DEPOSIT ,Categories::CLOTHES,100.0);
    user3.addTransaction(TransactionType::WITHDRAW,Categories::FOOD,150.0);
    user5.addTransaction(TransactionType::DEPOSIT,Categories::NONE,1500.0);
    user5.addTransaction(TransactionType::WITHDRAW,Categories::ELECTRONICS,500.0);
    
    db.printTransactions();

    SharedTransaction sh (600.0, Categories::FOOD, user1);
    sh.addParticipant(user2,150);
    sh.settleParticipantPayment(user2,50);
    sh.addParticipant(user3,150);

    db.printParticipants();

    user1.generateReport(Date(1,9,2024),Date(20,12,2024));
    


    return 0;
}
