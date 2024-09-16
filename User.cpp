#include "User.hpp"
#include "SharedTransaction.hpp"
#include <memory>
#include <algorithm>
#include "Logger.hpp"

using namespace std;

User::User(const string &userName) : userName(userName)
{
    Logger::getInstance()->log(Info, userName, " Program start.");
    DataBaseManager::getInstance().insertUser(*this);
}

User::~User()
{
    Logger::getInstance()->log(Info, userName, " Program End.");
}

const string &User::getUserName() const
{
    return userName;
}

int User::getUserId() const
{
    return userId;
}
void User::setUserId(int id)
{
    userId = id;
}


void User::setSavingPlan(double target, const Date &startDate, const Date &endDate)
{
    SavingPlan newPlan = SavingPlan(target, startDate, endDate, userName);
    DataBaseManager::getInstance().insertSavingPlan(userId, newPlan);
}
void User::deleteSavingPlan(int planId){
    DataBaseManager::getInstance().deleteSavingPlan(planId);
}
void User::updateSavingPlan(int planId, double newTarget,  Date &newStartDate,  Date &newEndDate){
    DataBaseManager::getInstance().updateSavingPlan(planId,newTarget, newStartDate, newEndDate);
}
void User::addSavingAmount(int planId , double newAmount){
    DataBaseManager::getInstance().UpdateSavingAmount(planId,newAmount);
}


void User::updateTransaction(int transactionId, TransactionType type, Categories category, double value){
    DataBaseManager::getInstance().updateTransaction(userId,transactionId, type, category, value);
}
void User::deleteTransaction(int transactionId){
    DataBaseManager::getInstance().deleteTransaction(transactionId);
    //Logger::getInstance()->log(Error, userName, "Transaction userId %d not found, can't delete.", transactionId);
}
void User::addTransaction(TransactionType type, Categories category, double value)
{
    if (type == TransactionType::DEPOSIT)
    {
        deposit(value, category);
    }
    else
    {
        withdraw(value, category);
    }
}
void User::deposit(double value, Categories category){
    Transaction newTransaction(value, category, TransactionType::DEPOSIT);
    DataBaseManager::getInstance().insertTransaction(userId,newTransaction);
}
void User::withdraw(double value, Categories category){

    if (isBudgetExceeded(category, value)){
        return;
    }
    if (calculateCurrentBalance() >= value){
        Transaction newTransaction(value, category, TransactionType::WITHDRAW);
        DataBaseManager::getInstance().insertTransaction(userId,newTransaction);
    }else{
        Logger::getInstance()->log(Error, userName, "Withdraw of %.2f $ failed. Exceeds total balance.", value);
    }
}



double User::calculateIncoms() const
{
    return DataBaseManager::getInstance().calculateTotalIncoms(userId);
}
double User::calculateIncoms(const Date &d1, const Date &d2) const
{
    double totalIncoms = 0.0;
    
    return totalIncoms;
}
double User::calculateExponses() const
{
   return DataBaseManager::getInstance().calculateTotalExponses(userId);
}
double User::calculateExponses(Categories category) const
{
    return DataBaseManager::getInstance().calculateTotalExponses(userId,category);
}
double User::calculateExponses(const Date &d1, const Date &d2) const
{
    double totalExponses = 0.0;
    return totalExponses;
}
double User::calculateExponses(const Date &d1, const Date &d2, Categories category) const
{
    double totalExponses = 0.0;
    return totalExponses;
}
double User::calculateCurrentBalance() const
{
    return calculateIncoms() - calculateExponses();
}



bool User::isBudgetExceeded(Categories category, double amount)const {
    DataBaseManager::getInstance().isBudgetExceeded(userId,category,amount);
}
void User::setBudget(Categories category, double budget)
{
    DataBaseManager::getInstance().setCategoryBudget(userId,category,budget);
}
const char *User::printCategory(Categories category) const
{
    switch (category)
    {
    case Categories::FOOD:
        return "FOOD";
    case Categories::CLOTHES:
        return "CLOTHES";
    case Categories::SHOES:
        return "SHOES";
    case Categories::ELECTRONICS:
        return "ELECTRONICS";
    case Categories::MISCELLANEOUS:
        return "MISCELLANEOUS";
    default:
        return "NONE";
    }
}



void User::generateReport(const Date &d1, const Date &d2) const{ 
    cout << "User : "<< userName <<"| "<<userId<<" Report from ";
    d1.display();
    cout<< " to "; 
    d2.display();
    cout << ":\n";
    cout << "           ==================\n";

    cout << "----> Transaction: \n";

    DataBaseManager::getInstance().generateTransactionReport(d1, d2, userId);

    cout << "\n***********************************************************\n\n";
    cout << "----> Shared Transaction: \n";

    DataBaseManager::getInstance().genarateSharedTransactionReport(d1, d2, userId);

    cout << "\n***********************************************************\n\n";
    cout << "----> Saving Plan: \n";
    
    DataBaseManager::getInstance().genarateSavingPlansReport(d1, d2, userId);
}
