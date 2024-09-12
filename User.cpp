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
void User::updateSavingPlan(int planId, double newTarget, const Date &newStartDate, const Date &newEndDate){
    DataBaseManager::getInstance().updateSavingPlan(planId,newTarget, newStartDate, newEndDate);
}
void User::addSavingAmount(int planId , double newAmount){
    DataBaseManager::getInstance().UpdateSavingAmount(planId,newAmount);
}


void User::updateTransaction(int transactionId, TransactionType type, Categories category, double value){
}
void User::deleteTransaction(int transactionId){
    Logger::getInstance()->log(Error, userName, "Transaction userId %d not found, can't delete.", transactionId);
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
        Transaction newTransaction(value, category, TransactionType::WITHDRAW);
        DataBaseManager::getInstance().insertTransaction(userId,newTransaction);
}


double User::calculateIncoms() const
{
    double totalIncoms = 0.0;
    return totalIncoms;
}
double User::calculateIncoms(const Date &d1, const Date &d2) const
{
    double totalIncoms = 0.0;
    
    return totalIncoms;
}
double User::calculateExponses() const
{
    double totalExponses = 0.0;
    return totalExponses;
}
double User::calculateExponses(Categories category) const
{
    double totalExponses = 0.0;
    
    return totalExponses;
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
double User::calculateTotal() const
{
    return calculateIncoms() - calculateExponses();
}


bool User::checkBudget(Categories category, double value) const
{
    return true;
}
bool User::isWarnBudget(Categories category) const
{
    return 0.0;
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
}


void User::joinSharedTransaction(SharedTransaction *sh) const{
}

