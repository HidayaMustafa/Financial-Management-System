#include "DataBaseManager.hpp"
#include <iostream>


using namespace std;

DataBaseManager* DataBaseManager::instance = nullptr;
mutex DataBaseManager::instanceMutex;

DataBaseManager& DataBaseManager::getInstance() {
    lock_guard<mutex> lock(mutex);  
    if (instance == nullptr) {
        instance = new DataBaseManager();
    }
    return *instance;
}

DataBaseManager::DataBaseManager() : dbName("Wallet.db"), db(nullptr) {
    int exit = sqlite3_open(dbName.c_str(), &db);
    if (exit) {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Opened database successfully." << endl;
        createUserTable();
        createCategoryBudgetTable();
        createSavingPlanTable();
        createTransactionTable();
        createParticipantTable();
    }
}

DataBaseManager::~DataBaseManager() {
    if (db) {
        sqlite3_close(db);
    }
}

bool DataBaseManager::executeSQL(const string& sql) {
    char *errMsg = nullptr;
    int exe = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
    if (exe != SQLITE_OK) {
        cerr << "SQL execution failed: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}



bool DataBaseManager::createUserTable() {
    const string sqlTable =
        "CREATE TABLE IF NOT EXISTS User ("
        "userId INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL);";
    return executeSQL(sqlTable);
}
void DataBaseManager::insertUser(User &user) {
    const string sql = "INSERT INTO User (username) VALUES ('" 
        + user.getUserName() + "');";
    executeSQL(sql);
    user.setUserId(sqlite3_last_insert_rowid(db));
}
void DataBaseManager::printUsers() {
    const string sql = "SELECT * FROM User;";
    char* errMsg = nullptr;
    string result;
    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int {
        string* output = static_cast<string*>(data);
        for (int i = 0; i < argc; i++) {
            *output += (argv[i] ? argv[i] : "NULL");
            if (i < argc - 1) {
                *output += " | ";
            }
        }
        *output += "\n";
        return 0;
    };

    int rc = sqlite3_exec(db, sql.c_str(), callback, &result, &errMsg);

    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    }else{
        cout << "Users :\n"<<result << endl;
    }
}



bool DataBaseManager::createCategoryBudgetTable() {
    const string sqlTable =
        "CREATE TABLE IF NOT EXISTS CategoryBudget ("
        "userId INTEGER NOT NULL, "
        "category INTEGER NOT NULL, "
        "budget REAL NOT NULL, "
        "FOREIGN KEY (userId) REFERENCES User(userId) ON DELETE CASCADE);";
    return executeSQL(sqlTable);
}
void DataBaseManager::setCategoryBudget(int userId, Categories category, double newBudget) {
    const string checkSql = "SELECT COUNT(*) FROM CategoryBudget WHERE userId = "
                            + to_string(userId) + " AND category = " + to_string(static_cast<int>(category))+ ";";

    char* errMsg = nullptr;
    double existingBudget = -1.0; 

    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int {
        if (argc > 0 && argv[0]) {
            double* budgetPtr = static_cast<double*>(data);
            *budgetPtr = atof(argv[0]);
        }
        return 0;
    };

    int rc = sqlite3_exec(db, checkSql.c_str(), callback, &existingBudget, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return;
    }

    if (existingBudget != -1.0){
        // add info
        updateCategoryBudget(userId, category, newBudget,existingBudget);
    } else {
        insertCategoryBudget(userId, category, newBudget);
    }
}
void DataBaseManager::insertCategoryBudget(int userId, Categories category, double budget) {
    const string sql = "INSERT INTO CategoryBudget (userId, category, budget) VALUES ("
        + to_string(userId) + ", '"
        + to_string(static_cast<int>(category)) + "', "
        + to_string(budget) + ");";
    executeSQL(sql);
}
void DataBaseManager::updateCategoryBudget(int userId, Categories category, double newBudget,double existingBudget) {
    if(existingBudget >= newBudget ){
        std::cout << "Can't set budget to a smaller value.";
    }else{
    const string sql = "UPDATE CategoryBudget SET budget = " 
        + to_string(newBudget) + " WHERE userId = " + to_string(userId) 
        + " AND category = '" + to_string(static_cast<int>(category)) + "';";
    executeSQL(sql);
    }
}



bool DataBaseManager::createSavingPlanTable() {
    const string sqlTable =
        "CREATE TABLE IF NOT EXISTS SavingPlan ("
        "planId INTEGER PRIMARY KEY AUTOINCREMENT, "
        "userId INTEGER, "
        "savingAmount REAL, "
        "target REAL, "
        "startDate DATE, "
        "endDate DATE, "
        "FOREIGN KEY (userId) REFERENCES User(userId) ON DELETE CASCADE);";
    return executeSQL(sqlTable);
}
void DataBaseManager::insertSavingPlan(int userId ,SavingPlan plan) {
    const string sql = "INSERT INTO SavingPlan (userId, savingAmount, target, startDate, endDate) VALUES (" 
        + to_string(userId) + ", " 
        + to_string(plan.getSavingAmount()) + ", " 
        + to_string(plan.getTarget()) + ", '" 
        + plan.getStartDate().toString() + "', '" 
        + plan.getEndDate().toString() + "');";
    executeSQL(sql);
    plan.setId(sqlite3_last_insert_rowid(db));
}
void DataBaseManager::printSavingPlans() {
    const string sql = "SELECT * FROM SavingPlan;";
    string result;
    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int {
        string* output = static_cast<string*>(data);
        for (int i = 0; i < argc; i++) {
            *output += (argv[i] ? argv[i] : "NULL");
            if (i < argc - 1) {
                *output += " "; 
            }
        }
        *output += "\n";
        return 0;
    };
    char *errMsg = nullptr;
    int exit = sqlite3_exec(db, sql.c_str(), callback, &result, &errMsg);
    if (exit != SQLITE_OK) {
        cerr << "Error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Saving Plans:\n" << result << endl;
    }
}
void DataBaseManager::updateSavingPlan(int planId, double newTarget, const Date &newStartDate, const Date &newEndDate) {
    const string checkplans = "SELECT COUNT(*) FROM SavingPlan WHERE planId = " + to_string(planId) + ";";

    char* errMsg = nullptr;
    int planExists = 0; 

    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int {
        int* exists = static_cast<int*>(data);
        if (argc > 0 && argv[0]) {
            *exists = atoi(argv[0]); 
        }
        return 0;
    };

    int rc = sqlite3_exec(db, checkplans.c_str(), callback, &planExists, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return;
    }

    if (planExists > 0) {
        const string updateSql = "UPDATE SavingPlan SET "
                                 "target = " + to_string(newTarget) + ", "
                                 "startDate = '" + newStartDate.toString() + "', "
                                 "endDate = '" + newEndDate.toString() + "' "
                                 "WHERE planId = " + to_string(planId) + ";";
        
        rc = sqlite3_exec(db, updateSql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            cerr << "SQL error in update: " << errMsg << endl;
            sqlite3_free(errMsg);
        } else {
            cout << "Saving plan with ID " << planId << " updated successfully." << endl;
        }
    } else {
        cout << "Saving plan with ID " << planId << " not found." << endl;
    }
}
void DataBaseManager::deleteSavingPlan(int planId) {
    const string checkplans = "SELECT COUNT(*), savingAmount, userId FROM SavingPlan WHERE planId = " + to_string(planId) + ";";
    
    char* errMsg = nullptr;
    int planExists = 0;
    double amount = 0.0;
    int userId = 0;

    void* data[3];
    data[0] = &planExists;
    data[1] = &amount;
    data[2] = &userId;

    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int {
        void** values = static_cast<void**>(data);  
        int* exists = static_cast<int*>(values[0]);
        double* amount = static_cast<double*>(values[1]);
        int* userId = static_cast<int*>(values[2]);

        if (argc > 0 && argv[0]) {
            *exists = atoi(argv[0]);
        }
        if (argc > 1 && argv[1]) {
            *amount = atof(argv[1]);
        }
        if (argc > 2 && argv[2]) {
            *userId = atoi(argv[2]);
        }

        return 0;
    };

    int rc = sqlite3_exec(db, checkplans.c_str(), callback, data, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return;
    }

    if (planExists > 0) {
        if(amount> 0.0){
            Transaction newTransaction(amount, Categories::NONE, TransactionType::DEPOSIT);
            insertTransaction(userId, newTransaction);
        }
        const string deleteSql = "DELETE FROM SavingPlan WHERE planId = " + to_string(planId) + ";";
        
        rc = sqlite3_exec(db, deleteSql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            cerr << "SQL error in delete plan: " << errMsg << endl;
            sqlite3_free(errMsg);
        } else {
            cout << "Saving plan with ID " << planId << " deleted successfully." << endl;
        }

    } else {
        cout << "Saving plan with ID " << planId << " not found." << endl;
    }
}
void DataBaseManager::UpdateSavingAmount(int planId, double newAmount) {
    const string checkPlans = "SELECT COUNT(*), savingAmount FROM SavingPlan WHERE planId = " + to_string(planId) + ";";

    char* errMsg = nullptr;
    int planExists = 0;
    double existingAmount = 0.0; 

    void* data[2];
    data[0] = &planExists;
    data[1] = &existingAmount;

    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int {
        void** values = static_cast<void**>(data);  
        int* exists = static_cast<int*>(values[0]);
        double* amount = static_cast<double*>(values[1]);

        if (argc > 0 && argv[0]) {
            *exists = atoi(argv[0]);
        }
        if (argc > 1 && argv[1]) {
            *amount = atof(argv[1]);
        }

        return 0;
    };

    int rc = sqlite3_exec(db, checkPlans.c_str(), callback, data, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return;
    }

    if (planExists > 0) {
        const string updateSql = "UPDATE SavingPlan SET "
                                 "savingAmount = " + to_string(existingAmount + newAmount) + " "
                                 "WHERE planId = " + to_string(planId) + ";";
        
        rc = sqlite3_exec(db, updateSql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            cerr << "SQL error in update: " << errMsg << endl;
            sqlite3_free(errMsg);
        } else {
            cout << "Saving plan with ID " << planId << " updated successfully." << endl;
        }
    } else {
        cout << "Saving plan with ID " << planId << " not found." << endl;
    }
}



bool DataBaseManager::createTransactionTable() {
    const string sqlTable =
        "CREATE TABLE IF NOT EXISTS \"Transaction\" ("
        "transactionId INTEGER PRIMARY KEY AUTOINCREMENT, "
        "userId INTEGER, "
        "amount REAL NOT NULL, "
        "category INTEGER, "      
        "type INTEGER NOT NULL, "          
        "date TEXT NOT NULL, "
        "FOREIGN KEY (userId) REFERENCES User(userId) ON DELETE CASCADE);";
    return executeSQL(sqlTable);
}
void DataBaseManager::insertTransaction(int userId, Transaction &transaction) {
    const string sql = "INSERT INTO \"Transaction\" (userId, amount, category, type, date) VALUES ("
        + to_string(userId) + ", "
        + to_string(transaction.getAmount()) + ", '"
        + to_string(static_cast<int>(transaction.getCategory())) + "', '"
        + to_string(static_cast<int>(transaction.getTransactionType())) + "', '"
        + transaction.getDate().toString() + "');";
    executeSQL(sql);
    transaction.setTranId(sqlite3_last_insert_rowid(db));
}
void DataBaseManager::printTransactions() {
    const string sql = "SELECT * FROM \"Transaction\";";
    
    string result;
    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int {
        string* output = static_cast<string*>(data);
        for (int i = 0; i < argc; i++) {
            *output += string(azColName[i]) + ": " + (argv[i] ? argv[i] : "NULL") + " ";
        }
        *output += "\n";
        return 0;
    };

    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), callback, &result, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "Error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Transactions:\n" << result << endl;
    }
}
void DataBaseManager::updateTransaction(int transactionId){
    if (findTransaction(transactionId)){

    }else{

    }
}
void DataBaseManager::deleteTransaction(int transactionId) {
    if (findTransaction(transactionId)) {
        const string deleteSql = "DELETE FROM \"Transaction\" WHERE transactionId = " + to_string(transactionId) + ";";

        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, deleteSql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            cerr << "SQL error in delete: " << errMsg << endl;
            sqlite3_free(errMsg);
        } else {
            cout << "Transaction deleted successfully." << endl;
        }
    } else {
        cout << "Transaction with ID " << transactionId << " not found." << endl;
    }
}

bool DataBaseManager::findTransaction(int transactionId){
    const string checkplans = "SELECT COUNT(*) FROM \"Transaction\" WHERE transactionId = " + to_string(transactionId) + ";";

    char* errMsg = nullptr;
    int transactionExists = 0; 

    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int {
        int* exists = static_cast<int*>(data);
        if (argc > 0 && argv[0]) {
            *exists = atoi(argv[0]); 
        }
        return 0;
    };

    int rc = sqlite3_exec(db, checkplans.c_str(), callback, &transactionExists, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return  (transactionExists > 0) ;
}


bool DataBaseManager::createParticipantTable() {
    const string sqlTable =
        "CREATE TABLE IF NOT EXISTS Participant ("
        "userId INTEGER NOT NULL, "
        "transactionId INTEGER NOT NULL, "
        "paidValue REAL NOT NULL, "
        "requiredValue REAL NOT NULL, "
        "PRIMARY KEY (userId, transactionId), "
        "FOREIGN KEY (userId) REFERENCES User(userId) ON DELETE CASCADE, "
        "FOREIGN KEY (transactionId) REFERENCES \"Transaction\"(transactionId) ON DELETE CASCADE);";
    return executeSQL(sqlTable);
}
void DataBaseManager::insertParticipant(int userId, int transactionId, Participant& participant) {
    const string sql = "INSERT INTO Participant (userId, transactionId, paidValue, requiredValue) VALUES ("
        + to_string(userId) + ", "
        + to_string(transactionId) + ", "
        + to_string(participant.getPaid()) + ", "
        + to_string(participant.getRequired()) + ");";
    executeSQL(sql);
}
void DataBaseManager::printParticipants() {
    const string sql = "SELECT * FROM Participant;";
    
    string result;
    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int {
        string* output = static_cast<string*>(data);
        for (int i = 0; i < argc; i++) {
            *output += (argv[i] ? argv[i] : "NULL");
            if (i < argc - 1) {
                *output += " | ";
            }
        }
        *output += "\n";
        return 0;
    };

    char *errMsg = nullptr;
    int exit = sqlite3_exec(db, sql.c_str(), callback, &result, &errMsg);
    if (exit != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Participants:\n" << result << endl;
    }
}
