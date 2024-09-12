#pragma once

#include <sqlite3.h>
#include <string>
#include <mutex>
#include "User.hpp"
#include "Participant.hpp"


using namespace std;

class SavingPlan;
class User;
class Participant;

class DataBaseManager {
private:
    static DataBaseManager* instance;
    static mutex instanceMutex;

    sqlite3* db;
    string dbName;
    bool executeSQL(const string& sql);

    DataBaseManager();

    DataBaseManager(const DataBaseManager&) = delete;
    DataBaseManager& operator=(const DataBaseManager&) = delete;

public:
    
    static DataBaseManager& getInstance();

    
    ~DataBaseManager();

    
    bool createUserTable();
    void insertUser(User &user);
    void printUsers();


    bool createCategoryBudgetTable() ;
    void setCategoryBudget(int userId, Categories category, double budget);
    void insertCategoryBudget(int userId, Categories category, double budget);
    void updateCategoryBudget(int userId, Categories category, double newBudget,double existingBudget);


    bool createSavingPlanTable();
    void insertSavingPlan(int userId , SavingPlan plan);
    void printSavingPlans();
    void updateSavingPlan(int planId, double newTarget, const Date &newStartDate, const Date &newEndDate);
    void deleteSavingPlan(int planId);
    void UpdateSavingAmount(int planId,double newAmount);

    bool createTransactionTable();
    void insertTransaction(int userId , Transaction &transaction) ;
    void printTransactions();
    void updateTransaction(int transactionId);
    void deleteTransaction(int transactionId);
    bool findTransaction(int transactionId);
    

    bool createParticipantTable();
    void insertParticipant(int userId, int sharedTransactionId, Participant& participant);
    void printParticipants() ;
};
