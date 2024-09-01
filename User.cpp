#include "User.hpp"
#include "SharedTransaction.hpp"
#include <memory>
#include <algorithm>


using namespace std;


User::User(const string& Name, int Id) : id(Id), name(Name)
{
    //Logger::getInstance()->log(Info, name, " Program start.");
}

User::~User()
{
    //Logger::getInstance()->log(Info, name, " Program End.");
}

const vector<Transaction>& User::getTransactions() const
{
    return transactions;
}


const string& User::getName()const
{
    return name;
}

int User::getId() const
{
    return id;
}

void User::setId(int i)
{
    id = i;
}

void User::setSavingPlan(int id, double target, const Date& startDate, const Date& endDate)
{
    plans.push_back(make_unique<SavingPlan>(id, target, startDate, endDate, name));
}

SavingPlan *User::getSavingPlan(int id)
{
    for (auto &plan : plans)
    {
        if (plan->getId() == id)
        {
            return plan.get();
        }
    }
    //Logger::getInstance()->log(Error, name, "Saving plan %d not found.", id);
    return nullptr;
}

void User::deleteSavingPlan(int Id)
{
    auto it = find_if(plans.begin(), plans.end(), [Id](const unique_ptr<SavingPlan> &plan)
                      { return plan->getId() == Id; });

    if (it != plans.end())
    {
        addTransaction(Id, TransactionType::DEPOSIT, Categories::NONE, (*it)->getSavingAmount());
        //Logger::getInstance()->log(Info, name, " Saving plan %d deleted successfully.", Id);
        plans.erase(it);
    }
    else
    {
        //Logger::getInstance()->log(Error, name, " Saving plan %d not found.", Id);
    }
}

void User::updateSavingPlan(int Id, double newTarget, const Date& newStartDate, const Date& newEndDate)
{
    auto plan = getSavingPlan(Id);
    if (plan)
    {
        plan->setTarget(newTarget);
        plan->setStartDate(newStartDate);
        plan->setEndDate(newEndDate);
        //Logger::getInstance()->log(Info, name, " Saving plan %d updated successfully.", Id);
    }
    else
    {
        //Logger::getInstance()->log(Error, name, "Saving plan %d not found.", Id);
    }
}

void User::setBudget(Categories category, double budget)
{
    if (categoryBudgets[category] < budget)
    {
        categoryBudgets[category] = budget;
        //Logger::getInstance()->log(Info, name, " Budget for %s set to %.2f $.", printCategory(category), budget);
    }
    else
    {
        //Logger::getInstance()->log(Error, name, " Can't set budget for %s to a smaller value.", printCategory(category));
    }
}

const char *User::printCategory(Categories category)const
{
    switch (category)
    {
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
    default:
        return "NONE";
    }
}


void User::updateTransaction(int transId, TransactionType type, Categories category, double value)
{
    for (auto &transaction : transactions)
    {
        if (transaction.getTranId() != transId)
        {
            continue;
        }

        TransactionType currentType = transaction.getTransactionType();
        double currentAmount = transaction.getAmount();

        if (currentType == type)
        {
            if (type == TransactionType::DEPOSIT)
            {
                if (currentAmount != value)
                {
                    transaction.setAmount(value);
                    transaction.setCategory(Categories::NONE);
                    transaction.updateDate();
                    //Logger::getInstance()->log(Info, name, " Transaction ID %d -- updated deposit to %.2f $ --> deposited successfully.", transId, value);
                }
            }
            else
            {
                double amountDifference = value - currentAmount;
                if (amountDifference < 0 || checkBudget(category, amountDifference))
                {
                    transaction.setAmount(value);
                    transaction.setCategory(category);
                    transaction.updateDate();
                    //Logger::getInstance()->log(Info, name, " Transaction ID %d -- updated withdrawal to %.2f $ --> withdrawn successfully from %s.", transId, value, printCategory(category));
                }
                else
                {
                    //Logger::getInstance()->log(Error, name, "Transaction ID %d -- failed to update.", transId);
                }
            }
        }
        else
        {
            if (type == TransactionType::DEPOSIT)
            {
                if (currentType == TransactionType::WITHDRAW)
                {
                    transaction.setTransactionType(TransactionType::DEPOSIT);
                    transaction.setAmount(value);
                    transaction.setCategory(Categories::MISCELLANEOUS);
                    transaction.updateDate();
                    //Logger::getInstance()->log(Info, name, " Transaction ID %d -- updated withdrawal to deposit %.2f $ successfully.", transId, value);
                }
            }
            else
            {
                if (checkBudget(category, value) && value < calculateTotal() - transaction.getAmount())
                {
                    transaction.setAmount(value);
                    transaction.setTransactionType(TransactionType::WITHDRAW);
                    transaction.setCategory(category);
                    transaction.updateDate();
                    if (isWarnBudget(category))
                    {
                        //Logger::getInstance()->log(Warn, name, " The budget in %s has decreased to below 80%% of its previous amount.", printCategory(category));
                    }
                    //Logger::getInstance()->log(Info, name, " Transaction ID %d -- updated deposit to withdrawal %.2f $ successfully from %s.", transId, value, printCategory(category));
                }
                else
                {
                    //Logger::getInstance()->log(Error, name, "Transaction ID %d -- failed to update deposit to withdrawal.", transId);
                }
            }
        }
        return;
    }
    //Logger::getInstance()->log(Error, name, "Transaction ID %d not found, can't update.", transId);
}

void User::deleteTransaction(int transId)
{
    for (auto i = transactions.begin(); i != transactions.end(); ++i)
    {
        if (i->getTranId() == transId)
        {
            if (i->getTransactionType() == TransactionType::DEPOSIT)
            {
                if (calculateTotal() - i->getAmount() < 0)
                {
                    //Logger::getInstance()->log(Error, name, "Transaction ID %d can't be deleted, would result in negative balance.", transId);
                    return;
                }
            }
            //Logger::getInstance()->log(Info, name, " Transaction ID %d deleted successfully.", transId);
            transactions.erase(i);
            return;
        }
    }
    //Logger::getInstance()->log(Error, name, "Transaction ID %d not found, can't delete.", transId);
}

void User::addTransaction(int transId, TransactionType type, Categories category, double value)
{
    if (type == TransactionType::DEPOSIT)
    {
        deposit(transId, value, category);
    }
    else
    {
        withdraw(transId, value, category);
    }
}

void User::deposit(int transId, double value, Categories category)
{
    transactions.push_back(Transaction(transId, value, category, TransactionType::DEPOSIT));
    //Logger::getInstance()->log(Info, name, " Transaction ID %d -- %.2f $ deposited successfully.", transId, value);
}

bool User::isWarnBudget(Categories category)const
{
    return 0.8 * categoryBudgets.at(category) < calculateExponses(category);
}

void User::withdraw(int transId, double value, Categories category)
{
    if (!checkBudget(category, value))
    {
        //Logger::getInstance()->log(Error, name, "Withdraw of %.2f $ failed from %s. Exceeds budget.", value, printCategory(category));
        return;
    }
    if (calculateTotal() >= value)
    {
        transactions.push_back(Transaction(transId, value, category, TransactionType::WITHDRAW));
        //Logger::getInstance()->log(Info, name, " Transaction ID %d -- %.2f $ withdrawn successfully from %s.", transId, value, printCategory(category));

        if (isWarnBudget(category))
        {
            //Logger::getInstance()->log(Warn, name, " The budget in %s has decreased to below 80%% of its previous amount.", printCategory(category));
        }
    }
    else
    {
        //Logger::getInstance()->log(Error, name, "Withdraw of %.2f $ failed. Exceeds total balance.", value);
    }
}

double User::calculateIncoms()const
{
    double totalIncoms = 0.0;
    for (auto &transaction : transactions)
    {
        if (transaction.getTransactionType() == TransactionType::DEPOSIT)
        {
            totalIncoms += transaction.getAmount();
        }
    }
    return totalIncoms;
}

double User::calculateIncoms(const Date& d1, const Date& d2)const
{
    double totalIncoms = 0.0;
    for (auto &transaction : transactions)
    {
        if (transaction.getTransactionType() == TransactionType::DEPOSIT && transaction.getDate() >= d1 && transaction.getDate() <= d2)
        {
            totalIncoms += transaction.getAmount();
        }
    }
    return totalIncoms;
}

double User::calculateExponses()const
{
    double totalExponses = 0.0;
    for (auto &transaction : transactions)
    {
        if (transaction.getTransactionType() == TransactionType::WITHDRAW)
        {
            totalExponses += transaction.getAmount();
        }
    }
    return totalExponses;
}

double User::calculateExponses(Categories category)const
{
    double totalExponses = 0.0;
    for (auto &transaction : transactions)
    {
        if (transaction.getTransactionType() == TransactionType::WITHDRAW && transaction.getCategory() == category)
        {
            totalExponses += transaction.getAmount();
        }
    }
    return totalExponses;
}

double User::calculateExponses(const Date& d1, const Date& d2)const
{
    double totalExponses = 0.0;
    for (auto &transaction : transactions)
    {
        if (transaction.getTransactionType() == TransactionType::WITHDRAW && transaction.getDate() >= d1 && transaction.getDate() <= d2)
        {
            totalExponses += transaction.getAmount();
        }
    }
    return totalExponses;
}

double User::calculateExponses(const Date& d1, const Date& d2, Categories category)const
{
    double totalExponses = 0.0;
    for (auto &transaction : transactions)
    {
        if (transaction.getTransactionType() == TransactionType::WITHDRAW && transaction.getCategory() == category && transaction.getDate() >= d1 && transaction.getDate() <= d2)
        {
            totalExponses += transaction.getAmount();
        }
    }
    return totalExponses;
}

double User::calculateTotal()const
{
    return calculateIncoms() - calculateExponses();
}

bool User::checkBudget(Categories category, double value)const
{
    return categoryBudgets.at(category) >= calculateExponses(category) + value;
}

void User::generateReport(const Date& d1, const Date& d2)const
{
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

    for (auto &pair : categoryBudgets)
    {
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

void User::joinSharedTransaction(SharedTransaction& sh)const{
    for (const auto& transaction : sharedTransactions) {
        if (transaction->getTranId() == sh.getTranId()) {
            return; 
        }
    }
    sharedTransactions.push_back(&sh);
}

void User::deleteSharedTransaction(SharedTransaction& transaction) {
    // Remove the transaction from the vector of pointers
    auto it = std::remove(sharedTransactions.begin(), sharedTransactions.end(), &transaction);
    sharedTransactions.erase(it, sharedTransactions.end());
}




void User::printSharedTransaction() const {
    std::cout << "Shared Transactions for User " << name << " (ID: " << id << "):\n";
    for (const auto& sh : sharedTransactions) {
        std::cout << "Shared Transaction ID: " << sh->getTranId()
                  << ", Amount: " << sh->getAmount()
                  << ", Category: " << printCategory(sh->getCategory()) << '\n';
        sh->printParticipants();
    }
    std::cout << "----------------------\n";
}