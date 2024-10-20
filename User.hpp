#pragma once

#include <string>
#include <vector>
#include <functional>
#include <algorithm> 
#include <iostream>
#include <unordered_map>
#include "Transaction.hpp"
#include "SharedTransaction.hpp" 
#include "SavingPlan.hpp"
#include "Logger.hpp"

using namespace std;

class SharedTransaction;

class User {
private:
    int userId;
    string userName;
    vector<unique_ptr<SavingPlan>> plans;
    unordered_map<Categories, double> categoryBudgets;
    mutable vector<Transaction*> transactions;

public:
    User(const string& Name, int Id);
    virtual ~User();

    const string& getUserName() const;

    int getUserId() const;
    void setUserId(int i);

    void setSavingPlan(int id, double target, const Date& startDate, const Date& endDate);
    SavingPlan* getSavingPlan(int id);
    void deleteSavingPlan(int Id);
    void updateSavingPlan(int Id, double newTarget, const Date& newStartDate, const Date& newEndDate);
    void setBudget(Categories category, double budget);
    const char* printCategory(Categories category) const;
    void updateTransaction(int transId, TransactionType type, Categories category, double value);
    void deleteTransaction(int transId);
    void addTransaction(int transId, TransactionType type, Categories category, double value);
    void deposit(int transId, double value, Categories category);
    bool isWarnBudget(Categories category) const;
    void withdraw(int transId, double value, Categories category);
    double calculateIncoms() const;
    double calculateIncoms(const Date& d1, const Date& d2) const;
    double calculateExponses() const;
    double calculateExponses(Categories category) const;
    double calculateExponses(const Date& d1, const Date& d2) const;
    double calculateExponses(const Date& d1, const Date& d2, Categories category) const;
    double calculateTotal() const;
    bool checkBudget(Categories category, double value) const;
    void generateReport(const Date& d1, const Date& d2) const;
    void joinSharedTransaction(SharedTransaction *sh)const ;
    void deleteSharedTransaction(SharedTransaction& transaction);
    void printSharedTransaction() const;
};
