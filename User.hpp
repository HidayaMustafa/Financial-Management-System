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
#include "DataBaseManager.hpp"

using namespace std;

class SharedTransaction;
class SavingPlan;

class User {
private:
    int userId;
    string userName;

public:
    User(const string& Name);
    virtual ~User();

    const string& getUserName() const;
    int getUserId() const;
    void setUserId(int id);

    void setSavingPlan(double target, const Date& startDate, const Date& endDate);
    void deleteSavingPlan(int planId);
    void updateSavingPlan(int planId, double newTarget, const Date& newStartDate, const Date& newEndDate);
    void addSavingAmount(int planId , double newAmount);

    void updateTransaction(int transId, TransactionType type, Categories category, double value);
    void deleteTransaction(int transId);
    void addTransaction(TransactionType type, Categories category, double value);
    void deposit( double value, Categories category);
    void withdraw(double value, Categories category);

    double calculateIncoms() const;
    double calculateIncoms(const Date& d1, const Date& d2) const;
    double calculateExponses() const;
    double calculateExponses(Categories category) const;
    double calculateExponses(const Date& d1, const Date& d2) const;
    double calculateExponses(const Date& d1, const Date& d2, Categories category) const;
    double calculateTotal() const;

    void setBudget(Categories category, double budget);
    bool isWarnBudget(Categories category) const;
    const char* printCategory(Categories category) const;
    bool checkBudget(Categories category, double value) const;


    void generateReport(const Date& d1, const Date& d2) const;


    void joinSharedTransaction(SharedTransaction *sh)const ;
};
