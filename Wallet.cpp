#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include "Transaction.cpp"
#include "Logger.cpp"

using namespace std;

class Wallet {
private:
    unordered_map<Categories, double> categoryBudgets;
    vector<Transaction> transactions;
    string userName;

public:
    Wallet(const string& user) : userName(user) {
        Logger::getInstance()->log(Info, userName, " Program start.");
    }

    ~Wallet() {
        Logger::getInstance()->log(Info, userName, " Program End.");
    }

    void setBudget(Categories category, double budget) {
        if (categoryBudgets[category] < budget) {
            categoryBudgets[category] = budget;
            Logger::getInstance()->log(Info, userName, " Budget for %s set to %.2f $.", printCategory(category), budget);
        } else {
            Logger::getInstance()->log(Error, userName, " Can't set budget for %s to a smaller value.", printCategory(category));
        }
    }

    const char* printCategory(Categories category) {
        switch (category) {
        case Categories::FOOD:
            return "Food";
        case Categories::CLOTHES:
            return "Clothes";
        case Categories::SHOES:
            return "Shoes";
        case Categories::ELECTRONICS:
            return "Electronics";
        case Categories::MISCELLANEOUS:
            return "Miscellaneous";
        }
        return "NONE";
    }

    void updateTransaction(int id, TransactionType type, Categories category, double value) {
        for (auto &transaction : transactions) {
            if (transaction.getId() != id) {
                continue;
            }

            TransactionType currentType = transaction.getTransactionType();
            double currentAmount = transaction.getAmount();

            if (currentType == type) {
                if (type == TransactionType:: DEPOSIT) {
                    if (currentAmount != value) {
                        transaction.setAmount(value);
                        transaction.setCategory(Categories::MISCELLANEOUS); // Use a suitable default category or adjust as needed
                        transaction.updateDate();
                        Logger::getInstance()->log(Info, userName, " Transaction ID %d -- updated deposit to %.2f $ --> deposited successfully.", id, value);
                    }
                } else {
                    double amountDifference = value - currentAmount;
                    if (amountDifference < 0 || checkBudget(category, amountDifference)) {
                        transaction.setAmount(value);
                        transaction.setCategory(category);
                        transaction.updateDate();
                        Logger::getInstance()->log(Info, userName, " Transaction ID %d -- updated withdrawal to %.2f $ --> withdrawn successfully from %s.", id, value, printCategory(category));
                    } else {
                        Logger::getInstance()->log(Error, userName, "Transaction ID %d -- failed to update.", id);
                    }
                }
            } else {
                if (type == TransactionType:: DEPOSIT) {
                    if (currentType == TransactionType::WITHDRAW) {
                        transaction.setTransactionType(TransactionType:: DEPOSIT);
                        transaction.setAmount(value);
                        transaction.setCategory(Categories::MISCELLANEOUS);
                        transaction.updateDate();
                        Logger::getInstance()->log(Info, userName, " Transaction ID %d -- updated withdrawal to deposit %.2f $ successfully.", id, value);
                    }
                } else {
                    if (checkBudget(category, value) && value < calculateTotal() - transaction.getAmount()) {
                        transaction.setAmount(value);
                        transaction.setTransactionType(TransactionType::WITHDRAW);
                        transaction.setCategory(category);
                        transaction.updateDate();
                        if (isWarnBudget(category)) {
                            Logger::getInstance()->log(Warn, userName, " The budget in %s has decreased to below 80%% of its previous amount.", printCategory(category));
                        }
                        Logger::getInstance()->log(Info, userName, " Transaction ID %d -- updated deposit to withdrawal %.2f $ successfully from %s.", id, value, printCategory(category));
                    } else {
                        Logger::getInstance()->log(Error, userName, "Transaction ID %d -- failed to update deposit to withdrawal.", id);
                    }
                }
            }
            return;
        }
        Logger::getInstance()->log(Error, userName, "Transaction ID %d not found, can't update.", id);
    }

    void deleteTransaction(int id) {
        for (auto i = transactions.begin(); i != transactions.end(); ++i) {
            if (i->getId() == id) {
                if (i->getTransactionType() == TransactionType:: DEPOSIT) {
                    if (calculateTotal() - i->getAmount() < 0) {
                        Logger::getInstance()->log(Error, userName, "Transaction ID %d can't be deleted, would result in negative balance.", id);
                        return;
                    }
                }
                Logger::getInstance()->log(Info, userName, " Transaction ID %d deleted successfully.", id);
                transactions.erase(i);
                return;
            }
        }
        Logger::getInstance()->log(Error, userName, "Transaction ID %d not found, can't delete.", id);
    }

    void addTransaction(int id, TransactionType type, Categories category, double value) {
        if (type == TransactionType:: DEPOSIT) {
            deposit(id, value, category);
        } else {
            withdraw(id, value, category);
        }
    }

    void deposit(int id, double value, Categories category) {
        transactions.push_back(Transaction(id, value, category, TransactionType:: DEPOSIT));
        Logger::getInstance()->log(Info, userName, " Transaction ID %d -- %.2f $ deposited successfully.", id, value);
    }

    bool isWarnBudget(Categories category) {
        return 0.8 * categoryBudgets[category] < calculateExponses(category);
    }

    void withdraw(int id, double value, Categories category) {
        if (!checkBudget(category, value)) {
            Logger::getInstance()->log(Error, userName, "Withdraw of %.2f $ failed from %s. Exceeds budget.", value, printCategory(category));
            return;
        }
        if (calculateTotal() >= value) {
            transactions.push_back(Transaction(id, value, category, TransactionType::WITHDRAW));
            Logger::getInstance()->log(Info, userName, " Transaction ID %d -- %.2f $ withdrawn successfully from %s.", id, value, printCategory(category));

            if (isWarnBudget(category)) {
                Logger::getInstance()->log(Warn, userName, " The budget in %s has decreased to below 80%% of its previous amount.", printCategory(category));
            }
        } else {
            Logger::getInstance()->log(Error, userName, "Withdraw of %.2f $ failed. Exceeds total balance.", value);
        }
    }

    double calculateIncoms() {
        double totalIncoms = 0.0;
        for (auto &transaction : transactions) {
            if (transaction.getTransactionType() == TransactionType:: DEPOSIT) {
                totalIncoms += transaction.getAmount();
            }
        }
        return totalIncoms;
    }

    double calculateIncoms(Date d1, Date d2) {
        double totalIncoms = 0.0;
        for (auto &transaction : transactions) {
            if (transaction.getTransactionType() == TransactionType:: DEPOSIT && transaction.getDate() >= d1 && transaction.getDate() <= d2) {
                totalIncoms += transaction.getAmount();
            }
        }
        return totalIncoms;
    }

    double calculateExponses() {
        double totalExponses = 0.0;
        for (auto &transaction : transactions) {
            if (transaction.getTransactionType() == TransactionType::WITHDRAW) {
                totalExponses += transaction.getAmount();
            }
        }
        return totalExponses;
    }

    double calculateExponses(Categories category) {
        double totalExponses = 0.0;
        for (auto &transaction : transactions) {
            if (transaction.getTransactionType() == TransactionType::WITHDRAW && transaction.getCategory() == category) {
                totalExponses += transaction.getAmount();
            }
        }
        return totalExponses;
    }

    double calculateExponses(Date d1, Date d2) {
        double totalExponses = 0.0;
        for (auto &transaction : transactions) {
            if (transaction.getTransactionType() == TransactionType::WITHDRAW && transaction.getDate() >= d1 && transaction.getDate() <= d2) {
                totalExponses += transaction.getAmount();
            }
        }
        return totalExponses;
    }

    double calculateExponses(Date d1, Date d2, Categories category) {
        double totalExponses = 0.0;
        for (auto &transaction : transactions) {
            if (transaction.getTransactionType() == TransactionType::WITHDRAW && transaction.getCategory() == category && transaction.getDate() >= d1 && transaction.getDate() <= d2) {
                totalExponses += transaction.getAmount();
            }
        }
        return totalExponses;
    }

    double calculateTotal() {
        return calculateIncoms() - calculateExponses();
    }

    bool checkBudget(Categories category, double value) {
        return categoryBudgets[category] >= calculateExponses(category) + value;
    }

    bool checkBudget(double value) {
        for (auto &pair : categoryBudgets) {
            if (!checkBudget(pair.first, value)) {
                return false;
            }
        }
        return true;
    }

    void generateReport(Date d1, Date d2) {
        double totalIncoms = calculateIncoms(d1, d2);
        double totalExponses = calculateExponses(d1, d2);
        double netTotal = totalIncoms - totalExponses;

        cout << "Report from ";
        d1.display();
        cout << " to ";
        d2.display();
        cout << ":\n";
        cout << "---------------------------------\n";
        cout << "Total Incoms: " << totalIncoms << "$\n";
        cout << "Total Exponses: " << totalExponses << "$\n";
        cout << "Net Total: " << netTotal << "$\n";
        cout << "---------------------------------\n";

        for (auto &pair : categoryBudgets) {
            Categories category = pair.first;
            double budget = pair.second;
            double Exponses = calculateExponses(d1, d2, category);
            cout << "Category: " << printCategory(category) << "\n";
            cout << "Budget: " << budget << "$\n";
            cout << "Exponses: " << Exponses << "$\n";
            cout << "Remaining Budget: " << budget - Exponses << "$\n";
            cout << "---------------------------------\n";
        }
    }
};
