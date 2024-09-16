#pragma once

#include <sqlite3.h>
#include <string>
#include <mutex>
#include "User.hpp"
#include "Participant.hpp"
#include "Date.hpp"


using namespace std;

class SavingPlan;
class User;
class Participant;
class Date;

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
    bool isBudgetExceeded(int userId, Categories category, double amount);
    void printCategoryBudgetTable() ;

    bool createSavingPlanTable();
    void insertSavingPlan(int userId , SavingPlan plan);
    void printSavingPlans();
    void updateSavingPlan(int planId, double newTarget,  Date &newStartDate,  Date &newEndDate);
    void deleteSavingPlan(int planId);
    void UpdateSavingAmount(int planId,double newAmount);

    bool createTransactionTable();
    void insertTransaction(int userId , Transaction &transaction) ;
    void printTransactions();
    void updateTransaction(int userId, int transactionId,TransactionType type, Categories category, double value);
    void deleteTransaction(int transactionId);
    

    bool createParticipantTable();
    void insertParticipant(int userId, int sharedTransactionId, Participant participant);
    void printParticipants() ;
    double calculateParticipantAmountRequired(int transactionId);
    void updateRequiredAmount(int transactionId ,double required);
    void updatePaidAmount(int transactionId,double paid);
    void settleParticipantPayment(int transactionId,int userId,double newValue);


    double calculateTotalIncoms(int userId);
    double calculateTotalExponses(int userId);
    double calculateTotalExponses(int userId,Categories category);


    const char *toStringCategory(Categories category) const;
    const char *toStringType(TransactionType type) const;


    vector<vector<string>> executeQuery(const string& query);
    void generateTransactionReport(const Date &d1, const Date &d2, int userId) ;
    void genarateSharedTransactionReport(const Date &d1, const Date &d2, int userId);
    void genarateSavingPlansReport(const Date &d1, const Date &d2, int userId);

};
