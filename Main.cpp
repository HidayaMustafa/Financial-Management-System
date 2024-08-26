#include <iostream>
#include "User.cpp"

using namespace std;

int main() {
    User user1("Hidaya Mustafa", 1);
    cout << "---> User: " << user1.getName() << ", ID: " << user1.getId() << "\n";
   
    user1.getWallet().setBudget(Categories::FOOD, 300);
    user1.getWallet().setBudget(Categories::SHOES, 300);
    
    user1.getWallet().addTransaction(0, TransactionType::DEPOSIT ,Categories::NONE, 300.0);
    user1.getWallet().addTransaction(1, TransactionType::WITHDRAW, Categories::FOOD, 200.0);

    user1.setSavingPlan(0, 200.0, Date(15, 8, 2024), Date(20, 8, 2024));
    
    user1.getSavingPlan(0)->transferAmount(100);
    user1.getSavingPlan(0)->checkSavingPlan();

    user1.updateSavingPlan(0, 100.0, Date(15, 8, 2024), Date(20, 8, 2024));
    user1.getSavingPlan(0)->checkSavingPlan();

    user1.setSavingPlan(1, 250.0, Date(11, 8, 2024), Date(20, 8, 2024));

    user1.deleteSavingPlan(0);

    user1.getWallet().generateReport(Date(11, 8, 2024), Date(21, 8, 2024));
    
    return 0;
}
