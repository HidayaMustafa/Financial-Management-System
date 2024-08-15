#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include "Transaction.cpp"
#include "Logger.cpp"
#include <thread>

using namespace std;

class Wallet
{
private:
    unordered_map<Categories, double> categoryBudgets;
    vector<Transaction> transactions;

public:
    Wallet(){
        Logger ::getInstance()-> log("\n\nProgram start .");
    }

    ~Wallet(){
        Logger ::getInstance()-> log("Program End .");
    }

    void setBudget(Categories category, double budget)
    {
        if (categoryBudgets[category] < budget)
        {
            categoryBudgets[category] = budget;
            Logger::getInstance()->log("Budget for %s set to  %.2f $.\n" , printCategory(category) , budget);
        }
        else
        {
            Logger::getInstance()->error("Can't set budget for %s to a smaller value.\n", printCategory(category));
        }
    }

   const char* printCategory(Categories category)
    {
        switch (category)
        {
        case Food:
            return "Food";
        case Clothes:
            return "Clothes";
        case Shoes:
            return "Shoes";
        case Electronics:
            return "Electronics";
        case Miscellaneous:
            return "Miscellaneous";
        case null:
            return "None";
        }
        return "";
    }

    void updateTransaction(int id, TransactionType type, Categories category, double value) {
        for (auto &transaction : transactions) {
            if (transaction.getId() != id) {
                continue;
            }

            TransactionType currentType = transaction.getTransactionType();
            double currentAmount = transaction.getAmount();

            if (currentType == type) {
                if (type == Deposit) {
                    if (currentAmount != value) {
                        transaction.setAmount(value);
                        transaction.setCategory(null);
                        transaction.updateDate();
                        Logger::getInstance()->log("Transaction ID %d -- updated deposit to %.2f $ --> deposited successfully.\n", id, value);
                    }
                } else {
                    double amountDifference = value - currentAmount;
                    if (amountDifference < 0 || checkBudget(category, amountDifference)) {
                        transaction.setAmount(value);
                        transaction.setCategory(category);
                        transaction.updateDate();
                        Logger::getInstance()->log("Transaction ID %d -- updated withdrawal to %.2f $ --> withdrawn successfully from %s.\n", id, value, printCategory(category));
                    } else {
                        Logger::getInstance()->error("Transaction ID %d -- failed to update .\n", id);
                    }
                }
            } else {
                if (type == Deposit) {
                    if (currentType == Withdraw) {
                        transaction.setTransactionType(Deposit);
                        transaction.setAmount(value);
                        transaction.setCategory(null);
                        transaction.updateDate();
                        Logger::getInstance()->log("Transaction ID %d -- updated withdrawal to deposit %.2f $ successfully.\n", id, value);
                    }
                } else {
                    if (checkBudget(category, value) && value < calculateTotal() - transaction.getAmount()) {
                        transaction.setAmount(value);
                        transaction.setTransactionType(Withdraw);
                        transaction.setCategory(category);
                        transaction.updateDate();
                        if(isWarnBudget(category)){
                            Logger::getInstance()->warn("The budget in %s has decreased to below 80%% of its previous amount.", printCategory(category));
                        }
                        Logger::getInstance()->log("Transaction ID %d -- updated deposit to withdrawal %.2f $ successfully from %s.\n", id, value, printCategory(category));
                    } else {
                        Logger::getInstance()->error("Transaction ID %d -- failed to update deposited to withdraw.\n", id);
                    }
                }
            }
            return;
        }
        Logger::getInstance()->error("Transaction ID %d not found, can't update.\n", id);
    }

    void deleteTransaction(int id) {
        for (auto i = transactions.begin(); i != transactions.end(); ++i) {
            if (i->getId() == id) {
                if(i->getTransactionType() == Deposit) {
                    if(calculateTotal() - i->getAmount() < 0) {
                        Logger::getInstance()->error("Transaction ID %d can't be deleted, would result in negative balance.\n", id);
                        return;
                    }
                }
                Logger::getInstance()->log("Transaction ID %d deleted successfully.\n", id);
                transactions.erase(i);
                return;
            }
        }
        Logger::getInstance()->error("Transaction ID %d not found, can't delete.\n", id);
    }

    void addTransaction(int id, TransactionType type, Categories category, double value) {
        if (type == Deposit) {
            deposit(id, value, category);
        } else {
            withdraw(id, value, category);
        }
    }

    void deposit(int id, double value, Categories category) {
        transactions.push_back(Transaction(id, value, category, Deposit));
        Logger::getInstance()->log("Transaction ID %d -- %.2f $ deposited successfully.\n", id, value);
    }

    bool isWarnBudget( Categories category){
        return  0.8 * categoryBudgets[category] < calculateExpenses(category) ;
    }

    void withdraw(int id, double value, Categories category) {
        if (!checkBudget(category, value)) {
            Logger::getInstance()->error("Withdraw of %.2f $ failed from %s. Exceeds budget.\n", value, printCategory(category));
            return;
        }
        if (calculateTotal() >= value) {
            transactions.push_back(Transaction(id, value, category, Withdraw));
            Logger::getInstance()->log("Transaction ID %d -- %.2f $ withdrawn successfully from %s.\n", id, value, printCategory(category));
            
            if(isWarnBudget(category)){
            Logger::getInstance()->warn("The budget in %s has decreased to below 80%% of its previous amount.", printCategory(category));
            }
        } else {
            Logger::getInstance()->error("Withdraw of %.2f $ failed. Exceeds total balance.\n", value);
        }
    }

    double calculateIncomes()
    {
        double totalIncomes = 0.0;
        for (auto &transaction : transactions)
        {
            if (transaction.getTransactionType() == Deposit)
            {
                totalIncomes += transaction.getAmount();
            }
        }
        return totalIncomes;
    }

    double calculateIncomes(Date d1, Date d2)
    {
        double totalIncomes = 0.0;
        for (auto &transaction : transactions)
        {
            if (transaction.getTransactionType() == Deposit && transaction.getDate() >= d1 && transaction.getDate() <= d2)
            {
                totalIncomes += transaction.getAmount();
            }
        }
        return totalIncomes;
    }

    double calculateExpenses()
    {
        double totalExpenses = 0.0;
        for (auto &transaction : transactions)
        {
            if (transaction.getTransactionType() == Withdraw)
            {
                totalExpenses += transaction.getAmount();
            }
        }
        return totalExpenses;
    }

    double calculateExpenses(Categories category)
    {
        double totalExpenses = 0.0;
        for (auto &transaction : transactions)
        {
            if (transaction.getTransactionType() == Withdraw && transaction.getCategory() == category)
            {
                totalExpenses += transaction.getAmount();
            }
        }
        return totalExpenses;
    }

    double calculateExpenses(Date d1, Date d2)
    {
        double totalExpenses = 0.0;
        for (auto &transaction : transactions)
        {
            if (transaction.getTransactionType() == Withdraw && transaction.getDate() >= d1 && transaction.getDate() <= d2)
            {
                totalExpenses += transaction.getAmount();
            }
        }
        return totalExpenses;
    }

    double calculateExpenses(Date d1, Date d2, Categories category)
    {
        double totalExpenses = 0.0;
        for (auto &transaction : transactions)
        {
            if (transaction.getTransactionType() == Withdraw && transaction.getCategory() == category && transaction.getDate() >= d1 && transaction.getDate() <= d2)
            {
                totalExpenses += transaction.getAmount();
            }
        }
        return totalExpenses;
    }

    double calculateTotal()
    {
        return calculateIncomes() - calculateExpenses();
    }

    bool checkBudget(Categories category, double value)
    {
        return categoryBudgets[category] >= calculateExpenses(category) + value;
    }
    bool checkBudget(double value)
    {
        for (auto &pair : categoryBudgets)
        {
            if (!checkBudget(pair.first, value))
            {
                return false;
            }
        }
        return true;
    }

    void generateReport(Date d1, Date d2)
    {
        double totalIncomes = calculateIncomes(d1, d2);
        double totalExpenses = calculateExpenses(d1, d2);
        double netTotal = totalIncomes - totalExpenses;

        cout << "Report from ";
        d1.display();
        cout << " to ";
        d2.display();
        cout << ":\n";
        cout << "---------------------------------\n";
        cout << "Total Incomes: " << totalIncomes << "$\n";
        cout << "Total Expenses: " << totalExpenses << "$\n";
        cout << "Net Total: " << netTotal << "$\n";
        cout << "---------------------------------\n";

        for (auto &pair : categoryBudgets)
        {
            Categories category = pair.first;
            double budget = pair.second;
            double expenses = calculateExpenses(d1, d2, category);
            cout << "Category: " << printCategory(category) << "\n";
            cout << "Budget: " << budget << "$\n";
            cout << "Expenses: " << expenses << "$\n";
            cout << "Remaining Budget: " << budget - expenses << "$\n";
            cout << "---------------------------------\n";
        }
    }
    void setSavingPlane(double target , Date startDate , Date endDate ){
        double currentAmount = calculateIncomes(startDate,endDate) - calculateExpenses(startDate,endDate);
        double amountToSave = target - currentAmount;

        if (amountToSave <= 0) {
            Logger::getInstance->log("Good job, You have met your saving target.\n");
            return;
        }
        int remainingDays = endDate - Date() ;
        if (remainingDays <=0 ){
            Logger::getInstance()->error("You did not meet your saving target within the time limit.\n");
        }else{
            Logger::getInstance()->warn("You still need to save %.2f$ to meet your target. %d days remaining.\n", amountToSave, remainingDays);
        }
    }
};
