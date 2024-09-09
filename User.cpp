#include "User.hpp"
#include "SharedTransaction.hpp"
#include <memory>
#include <algorithm>
#include "Logger.hpp"

using namespace std;

User::User(const string &Name, int Id) : id(Id), name(Name)
{
    Logger::getInstance()->log(Info, name, " Program start.");
}

User::~User()
{
    Logger::getInstance()->log(Info, name, " Program End.");
    plans.clear();
    
    for (auto *t : transactions) {
        if (t == nullptr) {
            continue; 
        }
        if (t->getIsShared() == 0) { 
            delete t; 
        } else if (t->getIsShared() == 1) {
            auto *sharedTransaction = static_cast<SharedTransaction *>(t);
                sharedTransaction->removeParticipant(*this);
                if (sharedTransaction->getParticipants().empty()) {
                    delete sharedTransaction;
                }
        }
    }

    transactions.clear();
}

const string &User::getName() const
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

void User::setSavingPlan(int id, double target, const Date &startDate, const Date &endDate)
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
    Logger::getInstance()->log(Error, name, "Saving plan %d not found.", id);
    return nullptr;
}

void User::deleteSavingPlan(int Id)
{
    auto it = find_if(plans.begin(), plans.end(), [Id](const unique_ptr<SavingPlan> &plan)
                      { return plan->getId() == Id; });

    if (it != plans.end())
    {
        addTransaction(Id, TransactionType::DEPOSIT, Categories::NONE, (*it)->getSavingAmount());
        Logger::getInstance()->log(Info, name, " Saving plan %d deleted successfully.", Id);
        plans.erase(it);
    }
    else
    {
        Logger::getInstance()->log(Error, name, " Saving plan %d not found.", Id);
    }
}

void User::updateSavingPlan(int Id, double newTarget, const Date &newStartDate, const Date &newEndDate)
{
    auto plan = getSavingPlan(Id);
    if (plan)
    {
        plan->setTarget(newTarget);
        plan->setStartDate(newStartDate);
        plan->setEndDate(newEndDate);
        Logger::getInstance()->log(Info, name, " Saving plan %d updated successfully.", Id);
    }
    else
    {
        Logger::getInstance()->log(Error, name, "Saving plan %d not found.", Id);
    }
}

void User::setBudget(Categories category, double budget)
{
    if (categoryBudgets[category] < budget)
    {
        categoryBudgets[category] = budget;
        Logger::getInstance()->log(Info, name, " Budget for %s set to %.2f $.", printCategory(category), budget);
    }
    else
    {
        Logger::getInstance()->log(Error, name, " Can't set budget for %s to a smaller value.", printCategory(category));
    }
}

const char *User::printCategory(Categories category) const
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
        if (transaction->getTranId() != transId)
        {
            continue;
        }

        TransactionType currentType = transaction->getTransactionType();
        double currentAmount = transaction->getAmount();

        if (currentType == type)
        {
            if (type == TransactionType::DEPOSIT)
            {
                if (currentAmount != value)
                {
                    transaction->setAmount(value);
                    transaction->setCategory(Categories::NONE);
                    transaction->updateDate();
                    Logger::getInstance()->log(Info, name, " Transaction ID %d -- updated deposit to %.2f $ --> deposited successfully.", transId, value);
                }
            }
            else
            {
                double amountDifference = value - currentAmount;
                if (amountDifference < 0 || checkBudget(category, amountDifference))
                {
                    transaction->setAmount(value);
                    transaction->setCategory(category);
                    transaction->updateDate();
                    Logger::getInstance()->log(Info, name, " Transaction ID %d -- updated withdrawal to %.2f $ --> withdrawn successfully from %s.", transId, value, printCategory(category));
                }
                else
                {
                    Logger::getInstance()->log(Error, name, "Transaction ID %d -- failed to update.", transId);
                }
            }
        }
        else
        {
            if (type == TransactionType::DEPOSIT)
            {
                if (currentType == TransactionType::WITHDRAW)
                {
                    transaction->setTransactionType(TransactionType::DEPOSIT);
                    transaction->setAmount(value);
                    transaction->setCategory(Categories::MISCELLANEOUS);
                    transaction->updateDate();
                    Logger::getInstance()->log(Info, name, " Transaction ID %d -- updated withdrawal to deposit %.2f $ successfully.", transId, value);
                }
            }
            else{
                if (checkBudget(category, value) && value < calculateTotal() - transaction->getAmount())
                {
                    transaction->setAmount(value);
                    transaction->setTransactionType(TransactionType::WITHDRAW);
                    transaction->setCategory(category);
                    transaction->updateDate();
                    if (isWarnBudget(category))
                    {
                        Logger::getInstance()->log(Warn, name, " The budget in %s has decreased to below 80%% of its previous amount.", printCategory(category));
                    }
                    Logger::getInstance()->log(Info, name, " Transaction ID %d -- updated deposit to withdrawal %.2f $ successfully from %s.", transId, value, printCategory(category));
                }
                else
                {
                    Logger::getInstance()->log(Error, name, "Transaction ID %d -- failed to update deposit to withdrawal.", transId);
                }
            }
        }
        return;
    }
    Logger::getInstance()->log(Error, name, "Transaction ID %d not found, can't update.", transId);
}

void User::deleteTransaction(int transId)
{
    for (auto i = transactions.begin(); i != transactions.end(); ++i)
    {
        if ((*i)->getTranId() == transId)
        {
            if ((*i)->getTransactionType() == TransactionType::DEPOSIT)
            {
                if (calculateTotal() - (*i)->getAmount() < 0)
                {
                    Logger::getInstance()->log(Error, name, "Transaction ID %d can't be deleted, would result in negative balance.", transId);
                    return;
                }
            }
            Logger::getInstance()->log(Info, name, " Transaction ID %d deleted successfully.", transId);
            delete (*i);
            transactions.erase(i);
            return;
        }
    }
    Logger::getInstance()->log(Error, name, "Transaction ID %d not found, can't delete.", transId);
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
    transactions.push_back(new Transaction(transId, value, category, TransactionType::DEPOSIT));
    Logger::getInstance()->log(Info, name, " Transaction ID %d -- %.2f $ deposited successfully.", transId, value);
}

bool User::isWarnBudget(Categories category) const
{
    return 0.8 * categoryBudgets.at(category) < calculateExponses(category);
}

void User::withdraw(int transId, double value, Categories category)
{
    if (!checkBudget(category, value))
    {
        Logger::getInstance()->log(Error, name, "Withdraw of %.2f $ failed from %s. Exceeds budget.", value, printCategory(category));
        return;
    }
    if (calculateTotal() >= value)
    {
        transactions.push_back(new Transaction(transId, value, category, TransactionType::WITHDRAW));
        Logger::getInstance()->log(Info, name, " Transaction ID %d -- %.2f $ withdrawn successfully from %s.", transId, value, printCategory(category));

        if (isWarnBudget(category))
        {
            Logger::getInstance()->log(Warn, name, " The budget in %s has decreased to below 80%% of its previous amount.", printCategory(category));
        }
    }
    else
    {
        Logger::getInstance()->log(Error, name, "Withdraw of %.2f $ failed. Exceeds total balance.", value);
    }
}

double User::calculateIncoms() const
{
    double totalIncoms = 0.0;
    for (auto &transaction : transactions)
    {
        if (transaction->getTransactionType() == TransactionType::DEPOSIT)
        {
            totalIncoms += transaction->getAmount();
        }
    }
    return totalIncoms;
}

double User::calculateIncoms(const Date &d1, const Date &d2) const
{
    double totalIncoms = 0.0;
    for (auto &transaction : transactions)
    {
        if (transaction->getTransactionType() == TransactionType::DEPOSIT && transaction->getDate() >= d1 && transaction->getDate() <= d2)
        {
            totalIncoms += transaction->getAmount();
        }
    }
    return totalIncoms;
}

double User::calculateExponses() const
{
    double totalExponses = 0.0;
    for (auto &transaction : transactions)
    {
        if (transaction->getTransactionType() == TransactionType::WITHDRAW)
        {
            SharedTransaction *sharedTransaction = dynamic_cast<SharedTransaction *>(transaction);
            if (sharedTransaction)
            {
                totalExponses += sharedTransaction->getParticipant(*this)->getPaid();
            }
            else
            {
                totalExponses += transaction->getAmount();
            }
        }
    }
    return totalExponses;
}

double User::calculateExponses(Categories category) const
{
    double totalExponses = 0.0;
    for (auto &transaction : transactions)
    {
        if (transaction->getTransactionType() == TransactionType::WITHDRAW && transaction->getCategory() == category)
        {
           SharedTransaction *sharedTransaction = dynamic_cast<SharedTransaction *>(transaction);
            if (sharedTransaction)
            {
                totalExponses += sharedTransaction->getParticipant(*this)->getPaid();
            }
            else
            {
                totalExponses += transaction->getAmount();
            }
        }
    }
    return totalExponses;
}

double User::calculateExponses(const Date &d1, const Date &d2) const
{
    double totalExponses = 0.0;
    for (auto &transaction : transactions)
    {
        if (transaction->getTransactionType() == TransactionType::WITHDRAW && transaction->getDate() >= d1 && transaction->getDate() <= d2)
        {
            SharedTransaction *sharedTransaction = dynamic_cast<SharedTransaction *>(transaction);
            if (sharedTransaction)
            {
                totalExponses += sharedTransaction->getParticipant(*this)->getPaid();
            }
            else
            {
                totalExponses += transaction->getAmount();
            }
        }
    }
    return totalExponses;
}

double User::calculateExponses(const Date &d1, const Date &d2, Categories category) const
{
    double totalExponses = 0.0;
    for (auto &transaction : transactions)
    {
        if (transaction->getTransactionType() == TransactionType::WITHDRAW && transaction->getCategory() == category && transaction->getDate() >= d1 && transaction->getDate() <= d2)
        {
            SharedTransaction *sharedTransaction = dynamic_cast<SharedTransaction *>(transaction);
            if (sharedTransaction)
            {
                totalExponses += sharedTransaction->getParticipant(*this)->getPaid();
            }
            else
            {
                totalExponses += transaction->getAmount();
            }
        }
    }
    return totalExponses;
}

double User::calculateTotal() const
{
    return calculateIncoms() - calculateExponses();
}

bool User::checkBudget(Categories category, double value) const
{
    return categoryBudgets.at(category) >= calculateExponses(category) + value;
}

void User::generateReport(const Date &d1, const Date &d2) const
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

void User::joinSharedTransaction(SharedTransaction *sh) const
{
    for (const auto &transaction : transactions)
    {
        if (transaction->getTranId() == sh->getTranId())
        {
            Logger::getInstance()->log(Warn, this->getName(), "User %s is already part of Shared Transaction ID: %d", name.c_str(), sh->getTranId());
            return;
        }
    }
    transactions.push_back(sh);
    Logger::getInstance()->log(Info, this->getName(), "User %s successfully joined Shared Transaction ID: %d.", name.c_str(), sh->getTranId());
}

void User::deleteSharedTransaction(SharedTransaction &transaction)
{
    auto it = std::remove(transactions.begin(), transactions.end(), &transaction);
    Logger::getInstance()->log(Info, this->getName(), "Shared Transaction ID: %d successfully removed for User %s", transaction.getTranId(), name.c_str());
    transactions.erase(it, transactions.end());
}

void User::printSharedTransaction() const
{
    std::cout << "Shared Transactions for User " << name << " (ID: " << id << "):\n";
    for (const auto &t : transactions)
    {
        SharedTransaction *sh = dynamic_cast<SharedTransaction *>(t);
        if (sh)
        {
            cout <<"ID:"<< sh->getTranId() << "-- Is Shared :" << sh->getIsShared() << "\n";
            std::cout << "\nShared Transaction ID: " << sh->getTranId()
                      << ", Amount: " << sh->getAmount()
                      << ", Category: " << printCategory(sh->getCategory()) << '\n';
            sh->printParticipants();
        }
    }
    std::cout << "______________________\n\n";


    for(auto it = transactions.begin(); it != transactions.end(); it++) {
        cout <<name<<"ID:"<< (*it)->getTranId() << "-- Is Shared :" << (*it)->getIsShared() << "\n";
    }
}