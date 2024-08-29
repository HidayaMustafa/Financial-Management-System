#pragma once

#include <string>
#include <vector>
#include <algorithm> 
#include <iostream>
#include <unordered_map>
#include "Transaction.hpp"
#include "SharedTransaction.hpp" 
// #include "Logger.hpp"
#include "SavingPlan.hpp"

using namespace std;

class SharedTransaction;

class User {
private:
    int id;
    string name;
    vector<unique_ptr<SavingPlan>> plans;
    unordered_map<Categories, double> categoryBudgets;
    vector<Transaction> transactions;
    vector<SharedTransaction> sharedTransactions;

public:
    User(string Name, int Id);
    ~User();

    const vector<Transaction>& getTransactions() const;

    string getName();
    void setName(string n);

    int getId() const;
    void setId(int i);

    void setSavingPlan(int id, double target, Date startDate, Date endDate);
    SavingPlan* getSavingPlan(int id);
    void deleteSavingPlan(int Id);
    void updateSavingPlan(int Id, double newTarget, Date newStartDate, Date newEndDate);

    void setBudget(Categories category, double budget);
    const char* printCategory(Categories category);

    void updateTransaction(int transId, TransactionType type, Categories category, double value);
    void deleteTransaction(int transId);
    void addTransaction(int transId, TransactionType type, Categories category, double value);

    void deposit(int transId, double value, Categories category);
    bool isWarnBudget(Categories category);
    void withdraw(int transId, double value, Categories category);

    double calculateIncoms();
    double calculateIncoms(Date d1, Date d2);
    double calculateExponses();
    double calculateExponses(Categories category);
    double calculateExponses(Date d1, Date d2);
    double calculateExponses(Date d1, Date d2, Categories category);
    double calculateTotal();
    bool checkBudget(Categories category, double value);

    void generateReport(Date d1, Date d2);

    SharedTransaction& createSharedTransaction(int tranId, double amount, Categories cat, int reqValue);
    vector<SharedTransaction>& getSharedTransaction() ;
    void printSh();
};
