#include "User.hpp"
#include "SharedTransaction.hpp"
#include <memory>
#include <algorithm>
#include "Logger.hpp"

using namespace std;

User::User(const string &userName, int userId) : id(userId), name(userName){
    Logger::getInstance()->log(INFO, userName, " Program start.");
}

User::~User(){
    Logger::getInstance()->log(INFO, userName, " Program End.");
    plans.clear();
    for (auto *transaction : transactions) {
        if (transaction == nullptr) {
            continue; 
        }
        if (transaction->getIsShared() == 0) { 
            delete transaction; 
        } else if (transaction->getIsShared() == 1) {
            auto *sharedTransaction = static_cast<SharedTransaction *>(transaction);
                sharedTransaction->removeParticipant(*this);
                if (sharedTransaction->getParticipants().empty()) {
                    delete sharedTransaction;
                }
        }
    }
    transactions.clear();
}
const string &User::getUseruserName() const{
    return userName;
}
int User::getUserId() const{
    return id;
}
void User::setUserId(int Id){
    userId = Id;
}


void User::setSavingPlan(int planId, double target, const Date &startDate, const Date &endDate){
    plans.push_back(make_unique<SavingPlan>(planId, target, startDate, endDate, userName));
}
SavingPlan *User::getSavingPlan(int planId){
    for (auto &plan : plans)
    {
        if (plan->getPlanId() == planId)
        {
            return plan.get();
        }
    }
    Logger::getInstance()->log(ERROR, userName, "Saving plan %d not found.", planId);
    return nullptr;
}
void User::deleteSavingPlan(int planId){
    auto it = find_if(plans.begin(), plans.end(), [planId](const unique_ptr<SavingPlan> &plan)
                      { return plan->getPlanId() == planId; });

    if (it != plans.end()){
        addTransaction(planId, TransactionType::DEPOSIT, Categories::NONE, (*it)->getSavingAmount());
        Logger::getInstance()->log(INFO, userName, " Saving plan %d deleted successfully.", planId);
        plans.erase(it);
    }else{
        Logger::getInstance()->log(ERROR, userName, " Saving plan %d not found.", planId);
    }
}
void User::updateSavingPlan(int planId, double newTarget, const Date &newStartDate, const Date &newEndDate){
    auto plan = getSavingPlan(planId);
    if (plan){
        plan->setTarget(newTarget);
        plan->setStartDate(newStartDate);
        plan->setEndDate(newEndDate);
        Logger::getInstance()->log(INFO, userName, " Saving plan %d updated successfully.", planId);
    }else{
        Logger::getInstance()->log(ERROR, userName, "Saving plan %d not found.", planId);
    }
}


bool User::checkBudget(Categories category, double amount) const{
    return categoryBudgets.at(category) >= calculateExponses(category) + amount;
}
bool User::isWarnBudget(Categories category) const
{
    return 0.8 * categoryBudgets.at(category) < calculateExponses(category);
}
void User::setBudget(Categories category, double budget){
    if (categoryBudgets[category] < budget){
        categoryBudgets[category] = budget;
        Logger::getInstance()->log(INFO, userName, " Budget for %s set to %.2f $.", printCategory(category), budget);
    }else{
        Logger::getInstance()->log(ERROR, userName, " Can't set budget for %s to a smaller value.", printCategory(category));
    }
}
const char *User::printCategory(Categories category) const{
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


void User::updateTransaction(int transactionId, TransactionType transactionType, Categories category, double value){
    for (auto &transaction : transactions)
    {
        if (transaction->getTranId() != transactionId)
        {
            continue;
        }

        TransactionType currentType = transaction->getTransactionType();
        double currentAmount = transaction->getAmount();

        if (currentType == transactionType)
        {
            if (transactionType == TransactionType::DEPOSIT)
            {
                if (currentAmount != value)
                {
                    transaction->setAmount(value);
                    transaction->setCategory(Categories::NONE);
                    transaction->updateDate();
                    Logger::getInstance()->log(INFO, userName, " Transaction ID %d -- updated deposit to %.2f $ --> deposited successfully.", transactionId, value);
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
                    Logger::getInstance()->log(INFO, userName, " Transaction ID %d -- updated withdrawal to %.2f $ --> withdrawn successfully from %s.", transactionId, value, printCategory(category));
                }
                else
                {
                    Logger::getInstance()->log(ERROR, userName, "Transaction ID %d -- failed to update.", transactionId);
                }
            }
        }
        else
        {
            if (transactionType == TransactionType::DEPOSIT)
            {
                if (currentType == TransactionType::WITHDRAW)
                {
                    transaction->setTransactionType(TransactionType::DEPOSIT);
                    transaction->setAmount(value);
                    transaction->setCategory(Categories::MISCELLANEOUS);
                    transaction->updateDate();
                    Logger::getInstance()->log(INFO, userName, " Transaction ID %d -- updated withdrawal to deposit %.2f $ successfully.", transactionId, value);
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
                        Logger::getInstance()->log(WARN, userName, " The budget in %s has decreased to below 80%% of its previous amount.", printCategory(category));
                    }
                    Logger::getInstance()->log(INFO, userName, " Transaction ID %d -- updated deposit to withdrawal %.2f $ successfully from %s.", transactionId, value, printCategory(category));
                }
                else
                {
                    Logger::getInstance()->log(ERROR, userName, "Transaction ID %d -- failed to update deposit to withdrawal.", transactionId);
                }
            }
        }
        return;
    }
    Logger::getInstance()->log(ERROR, userName, "Transaction ID %d not found, can't update.", transactionId);
}
void User::deleteTransaction(int transactionId){
    for (auto i = transactions.begin(); i != transactions.end(); ++i){
        if ((*i)->getTranId() == transactionId){
            if ((*i)->getTransactionType() == TransactionType::DEPOSIT && calculateTotal() - (*i)->getAmount() < 0){
                Logger::getInstance()->log(ERROR, userName, "Transaction ID %d can't be deleted, would result in negative balance.", transactionId);
                return;
            }
            Logger::getInstance()->log(INFO, userName, " Transaction ID %d deleted successfully.", transactionId);
            delete (*i);
            transactions.erase(i);
            return;
        }
    }
    Logger::getInstance()->log(ERROR, userName, "Transaction ID %d not found, can't delete.", transactionId);
}
void User::addTransaction(int transactionId, TransactionType transactionType, Categories category, double value){
    if (transactionType == TransactionType::DEPOSIT)
    {
        deposit(transactionId, value, category);
    }
    else
    {
        withdraw(transactionId, value, category);
    }
}
void User::deposit(int transactionId, double value, Categories category)
{
    transactions.push_back(new Transaction(transactionId, value, category, TransactionType::DEPOSIT));
    Logger::getInstance()->log(INFO, userName, " Transaction ID %d -- %.2f $ deposited successfully.", transactionId, value);
}
void User::withdraw(int transactionId, double value, Categories category)
{
    if (!checkBudget(category, value))
    {
        Logger::getInstance()->log(ERROR, userName, "Withdraw of %.2f $ failed from %s. Exceeds budget.", value, printCategory(category));
        return;
    }
    if (calculateTotal() >= value)
    {
        transactions.push_back(new Transaction(transactionId, value, category, TransactionType::WITHDRAW));
        Logger::getInstance()->log(INFO, userName, " Transaction ID %d -- %.2f $ withdrawn successfully from %s.", transactionId, value, printCategory(category));

        if (isWarnBudget(category))
        {
            Logger::getInstance()->log(WARN, userName, " The budget in %s has decreased to below 80%% of its previous amount.", printCategory(category));
        }
    }
    else
    {
        Logger::getInstance()->log(ERROR, userName, "Withdraw of %.2f $ failed. Exceeds total balance.", value);
    }
}

double User::calculateTotal() const
{
    double total = 0.0;
    for (const auto &transaction : transactions)
    {
        total += (transaction->getTransactionType() == TransactionType::DEPOSIT) ? transaction->getAmount() : -transaction->getAmount();
    }
    return total;
}
double User::calculateExponses(Categories category) const
{
    double totalExpenses = 0.0;
    for (const auto &transaction : transactions)
    {
        if (transaction->getTransactionType() == TransactionType::WITHDRAW && transaction->getCategory() == category)
        {
            totalExpenses += transaction->getAmount();
        }
    }
    return totalExpenses;
}