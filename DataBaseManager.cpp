#include "DataBaseManager.hpp"
#include <iostream>
#include "Logger.hpp"

using namespace std;

DataBaseManager* DataBaseManager::instance = nullptr;
mutex DataBaseManager::instanceMutex;

DataBaseManager& DataBaseManager::getInstance() {
    lock_guard<mutex> lock(instanceMutex);
    if (instance == nullptr) {
        instance = new DataBaseManager();
    }
    return *instance;
}


DataBaseManager::DataBaseManager() : dbName("Wallet.db"), db(nullptr) {
    int exit = sqlite3_open(dbName.c_str(), &db);
    if (exit) {
        Logger::getInstance()->log(LogLevel::ERROR,"", "Cannot open database: %s", sqlite3_errmsg(db));
    } else {
        Logger::getInstance()->log(LogLevel::INFO, "", "Opened database successfully: %s", dbName.c_str());
        
        createUserTable();
        createCategoryBudgetTable();
        createSavingPlanTable();
        createTransactionTable();
        createParticipantTable();
    }
}

DataBaseManager::~DataBaseManager() {
    if (db) {
        Logger::getInstance()->log(LogLevel::INFO, " ", "Closed database: %s", dbName.c_str());
        sqlite3_close(db);
    }
}

bool DataBaseManager::executeSQL(const string& sql) {
    char *errMsg = nullptr;
    int exe = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
    if (exe != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}


bool DataBaseManager::createUserTable() {
    const string sqlTable =
        "CREATE TABLE IF NOT EXISTS User ("
        "userId INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT UNIQUE NOT NULL);";
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
        "PRIMARY KEY (userId, category),"
        "FOREIGN KEY (userId) REFERENCES User(userId) ON DELETE CASCADE);";
    return executeSQL(sqlTable);
}
void DataBaseManager::setCategoryBudget(int userId, Categories category, double newBudget) {
    const string checkSql = "SELECT COUNT(*), budget FROM CategoryBudget WHERE userId = "
                            + to_string(userId) + " AND category = " + to_string(static_cast<int>(category)) + ";";

    char* errMsg = nullptr;
    int budgetExists = 0;
    double amount = 0.0;
    
    void* data[2];
    data[0] = &budgetExists; 
    data[1] = &amount; 

    auto callback = [](void* data, int argc, char** argv, char** azColName) -> int {
        void** values = static_cast<void**>(data);  
        
        int* budgetExistsPtr = static_cast<int*>(values[0]); 
        double* amountPtr = static_cast<double*>(values[1]); 
        
        if (argc > 0 && argv[0]) {
            *budgetExistsPtr = atoi(argv[0]); 
        }
        if (argc > 1 && argv[1]) {
            *amountPtr = atof(argv[1]); 
        }
        return 0;
    };

    int rc = sqlite3_exec(db, checkSql.c_str(), callback, data, &errMsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
        return;
    }

    if (budgetExists > 0) {
        updateCategoryBudget(userId, category, newBudget, amount); 
    } else {
        insertCategoryBudget(userId, category, newBudget);
    }
}
void DataBaseManager::insertCategoryBudget(int userId, Categories category, double budget) {
    const string sql = "INSERT INTO CategoryBudget (userId, category, budget) VALUES ("
        + to_string(userId) + ","
        + to_string(static_cast<int>(category)) + ", "
        + to_string(budget) + ");";

    bool rc = executeSQL(sql); 

    if (!rc) { 
        Logger::getInstance()->log(LogLevel::ERROR, 
            "User Id: " + to_string(userId), 
            "Failed to insert category budget for category: %s.", 
            toStringCategory(category));
    } else { 
        Logger::getInstance()->log(LogLevel::INFO, 
            "User Id: " + to_string(userId), 
            "Successfully inserted category budget for category: %s.", 
            toStringCategory(category));
    }
}
void DataBaseManager::updateCategoryBudget(int userId, Categories category, double newBudget, double existingBudget) {
    if (existingBudget >= newBudget) {
        Logger::getInstance()->log(LogLevel::ERROR, 
            "User Id: " + to_string(userId) , " Failed to update category budget for category: %s ,Cannot set a value lower than the current one.", toStringCategory(category));
    }else{
        const string sql = "UPDATE CategoryBudget SET budget = " 
            + to_string(newBudget) + " WHERE userId = " + to_string(userId) 
            + " AND category = '" + to_string(static_cast<int>(category)) + "';";

        bool rc = executeSQL(sql);
        if (!rc) {
            Logger::getInstance()->log(LogLevel::ERROR, 
                "User Id: " + to_string(userId) , " Failed to update category budget for category: %s.", toStringCategory(category));
        }else{
            Logger::getInstance()->log(LogLevel::INFO, 
                "User Id: " + to_string(userId) , " Update category budget for category: %s to: %0.2f.", toStringCategory(category),newBudget);
        }
    }
}
bool DataBaseManager::isBudgetExceeded(int userId, Categories category, double amount) {
    const string budgetQuery = "SELECT budget FROM CategoryBudget WHERE userId = " + to_string(userId) +
                               " AND category = " + to_string(static_cast<int>(category)) + ";";

    double budget = 0.0;
    char* errMsg = nullptr;

    auto budgetCallback = [](void* data, int argc, char** argv, char** azColName) -> int {
        if (argc > 0 && argv[0]) {
            *(static_cast<double*>(data)) = atof(argv[0]);
        }
        return 0;
    };

    int rc = sqlite3_exec(db, budgetQuery.c_str(), budgetCallback, &budget, &errMsg);
    if (rc != SQLITE_OK) {
        Logger::getInstance()->log(LogLevel::ERROR, 
            "User Id: " + to_string(userId) , " Failed to query budget for category:%s . Error:%s." ,toStringCategory(category) , errMsg);
        sqlite3_free(errMsg);
        return false;
    }

    if (calculateTotalExponses(userId, category) + amount > budget) {
        return true;
    }

    return false;
}
void DataBaseManager::printCategoryBudgetTable()  {
    const string sqlQuery = "SELECT * FROM CategoryBudget;";
    
    char* errMsg = nullptr;

    auto callback = [](void*, int argc, char** argv, char** azColName) -> int {
        for (int i = 0; i < argc; i++) {
            cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << " | ";
        }
        cout << endl;
        return 0;
    };

    int rc = sqlite3_exec(db, sqlQuery.c_str(), callback, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
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
void DataBaseManager::insertSavingPlan(int userId, SavingPlan plan) {
    const string sql = "INSERT INTO SavingPlan (userId, savingAmount, target, startDate, endDate) VALUES (" 
        + to_string(userId) + ", " 
        + to_string(plan.getSavingAmount()) + ", " 
        + to_string(plan.getTarget()) + ", '" 
        + plan.getStartDate().toString() + "', '" 
        + plan.getEndDate().toString() + "');";

    bool success = executeSQL(sql); 

    if (success) {
        plan.setId(sqlite3_last_insert_rowid(db)); 
        Logger::getInstance()->log(LogLevel::INFO, 
            "User Id: " + to_string(userId), 
            "Inserted new saving plan ID: %d  with target:%0.2f. " ,plan.getId(),plan.getTarget());
    } else {
        Logger::getInstance()->log(LogLevel::ERROR, 
            "User Id: " + to_string(userId), 
            "Failed to insert new saving plan with target:%0.2f ." ,plan.getTarget());
    }
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
void DataBaseManager::updateSavingPlan(int userId, int planId, double newTarget, const Date &newStartDate, const Date &newEndDate) {
    char* errMsg = nullptr;
    const string updateSql = "UPDATE SavingPlan SET "
                                 "target = " + to_string(newTarget) + ", "
                                 "startDate = '" + newStartDate.toString() + "', "
                                 "endDate = '" + newEndDate.toString() + "' "
                                 "WHERE planId = " + to_string(planId) + " AND userId = " + to_string(userId) + ";";

    int rc = sqlite3_exec(db, updateSql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        Logger::getInstance()->log(LogLevel::ERROR, 
            "User Id: " + to_string(userId) , " Failed to update saving plan with ID:%d . Error:%s ." ,planId, string(errMsg));
        sqlite3_free(errMsg);
    } else {
        Logger::getInstance()->log(LogLevel::INFO, 
            "User Id: " + to_string(userId) , " Updated saving plan with ID:%d ." ,planId);
    }
}
void DataBaseManager::deleteSavingPlan(int planId, int userId) {
    const string checkplans = "SELECT COUNT(*), savingAmount FROM SavingPlan WHERE planId = " + to_string(planId) + ";";
    
    char* errMsg = nullptr;
    int planExists = 0;
    double amount = 0.0;
    
    void* data[2];
    data[0] = &planExists;
    data[1] = &amount;
  
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

    int rc = sqlite3_exec(db, checkplans.c_str(), callback, data, &errMsg);
    if (rc != SQLITE_OK) {
        Logger::getInstance()->log(LogLevel::ERROR, 
            "User Id: " + to_string(userId) , " Failed to check saving plan existence for ID:%d . Error:%s." ,planId ,string(errMsg));
        sqlite3_free(errMsg);
        return;
    }

    if (planExists > 0) {
        if (amount > 0.0) {
            Transaction newTransaction(amount, Categories::NONE, TransactionType::DEPOSIT);
            insertTransaction(userId, newTransaction);
        }
        
        const string deleteSql = "DELETE FROM SavingPlan WHERE planId = " + to_string(planId) + ";";
        rc = sqlite3_exec(db, deleteSql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            Logger::getInstance()->log(LogLevel::ERROR, 
                "User Id: " + to_string(userId) , " Failed to delete saving plan with ID:%d . Error:%s." ,planId, string(errMsg));
            sqlite3_free(errMsg);
        } else {
            Logger::getInstance()->log(LogLevel::INFO, 
                "User Id: " + to_string(userId) , " Deleted saving plan with ID:%d " ,planId);
                  }
    } else {
        Logger::getInstance()->log(LogLevel::ERROR, 
            "User Id: " + to_string(userId) , " Saving plan with ID:%d  not found." ,planId);
     }
}
void DataBaseManager::UpdateSavingAmount(int userId, int planId, double newAmount) {
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
        sqlite3_free(errMsg);
        return;
    }

    if (planExists > 0) {
        const string updateSql = "UPDATE SavingPlan SET "
                                 "savingAmount = " + to_string(existingAmount + newAmount) + " "
                                 "WHERE planId = " + to_string(planId) + ";";
        
        rc = sqlite3_exec(db, updateSql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            Logger::getInstance()->log(LogLevel::ERROR, 
                "User Id: " + to_string(userId) , " Failed to update saving amount for plan ID:%d. Error:%s.",planId
                ,string(errMsg));
            sqlite3_free(errMsg);
        } else {
            Logger::getInstance()->log(LogLevel::INFO, 
                "User Id: " + to_string(userId) , " Updated saving amount for plan ID:%d to %0.2f." ,planId,existingAmount + newAmount);
        }
    } else {
        Logger::getInstance()->log(LogLevel::ERROR, 
            "User Id: " + to_string(userId), " Saving plan ID:%d  not found." ,planId);
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
        + to_string(transaction.getAmount()) + ", "
        + to_string(static_cast<int>(transaction.getCategory())) + ", "
        + to_string(static_cast<int>(transaction.getTransactionType())) + ", '"
        + transaction.getDate().toString() + "');";
    bool success = executeSQL(sql);

    transaction.setTranId(sqlite3_last_insert_rowid(db));
    
    if(userId == 0 && success){
        Logger::getInstance()->log(LogLevel::INFO, " ", 
                    "Shared Transaction ID: %d created successfully for amount %.2f in category %s",
                        sqlite3_last_insert_rowid(db), transaction.getAmount(), toStringCategory(transaction.getCategory()));
    
    }else if(userId != 0 && success){
        Logger::getInstance()->log(LogLevel::INFO, "User Id: "+to_string(userId), 
                    "Transaction ID: %d created successfully for amount %.2f in category %s",
                        sqlite3_last_insert_rowid(db), transaction.getAmount(), toStringCategory(transaction.getCategory()));
    
    }else if(!success){
         Logger::getInstance()->log(LogLevel::INFO, "User Id: "+to_string(userId), 
                    "Failed to create Transaction ID: %d.",sqlite3_last_insert_rowid(db));
    }
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
      sqlite3_free(errMsg);
    } else {
    cout << "Transactions:\n" << result << endl;
    }
}
void DataBaseManager::updateTransaction(int userId, int transactionId, TransactionType type, Categories category, double value) {
    int currentType = 0;
    double currentAmount = 0.0;
    int currentCategory = 0;

    void* data[3];
    data[0] = &currentType;
    data[1] = &currentAmount;
    data[2] = &currentCategory;

    string selectQuery = "SELECT type, amount, category FROM \"Transaction\" WHERE transactionId = " + to_string(transactionId) +
                         " AND userId = " + to_string(userId) + ";";

    auto callback = [](void* data, int argc, char** argv, char** azColName) -> int {
        void** values = static_cast<void**>(data);
        int* typePtr = static_cast<int*>(values[0]);
        double* amountPtr = static_cast<double*>(values[1]);
        int* categoryPtr = static_cast<int*>(values[2]);

        if (argc > 2 && argv[0] && argv[1] && argv[2]) {
            *typePtr = atoi(argv[0]);
            *amountPtr = atof(argv[1]);
            *categoryPtr = atoi(argv[2]);
        }
        return 0;
    };

    int rc = sqlite3_exec(db, selectQuery.c_str(), callback, data, nullptr);
    if (rc != SQLITE_OK) {
       return;
    }
    Logger::getInstance()->log(LogLevel::INFO, "User Id :" + to_string(userId),
        "Transaction ID: %d updated successfully with new amount: %.2f, new category: %s",
        transactionId, value, toStringCategory(category));
}
void DataBaseManager::deleteTransaction(int userId, int transactionId) {
    const string deleteSql = "DELETE FROM \"Transaction\" WHERE transactionId = " + to_string(transactionId) + ";";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, deleteSql.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        Logger::getInstance()->log(LogLevel::ERROR, "Failed to delete transaction",
            "Transaction ID: %d, User ID: %d, Error: %s", transactionId, userId, errMsg);
        sqlite3_free(errMsg);
    } else {
        Logger::getInstance()->log(LogLevel::INFO, "Transaction deleted",
            "Transaction ID: %d deleted successfully for User ID: %d", transactionId, userId);
    }
}


bool DataBaseManager::createParticipantTable() {
    const string sqlTable =
        "CREATE TABLE IF NOT EXISTS Participant ("
        "userId INTEGER NOT NULL, "
        "transactionId INTEGER NOT NULL, "
        "paidAmount REAL NOT NULL, "
        "requiredAmount REAL NOT NULL, "
        "PRIMARY KEY (userId, transactionId), "
        "FOREIGN KEY (userId) REFERENCES User(userId) ON DELETE CASCADE, "
        "FOREIGN KEY (transactionId) REFERENCES \"Transaction\"(transactionId) ON DELETE CASCADE);";
    return executeSQL(sqlTable);
}
void DataBaseManager::insertParticipant(int userId, int transactionId, Participant participant) {
    const string sql = "INSERT INTO Participant (userId, transactionId, paidAmount, requiredAmount) VALUES ("
        + to_string(userId) + ", "
        + to_string(transactionId) + ", "
        + to_string(participant.getPaid()) + ", "
        + to_string(participant.getRequired()) + ");";
    
    Logger::getInstance()->log(LogLevel::INFO, "User Id :"+to_string(userId), "Inserting participant into TransactionID: %d, Paid: %.2f, Required: %.2f", 
                                transactionId, participant.getPaid(), participant.getRequired());
    
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
double DataBaseManager::calculateParticipantAmountRequired(int transactionId) {
    const string query = 
        "SELECT T.amount, COUNT(P.userId) AS participantCount "
        "FROM \"Transaction\" T "
        "LEFT JOIN Participant P ON T.transactionId = P.transactionId "
        "WHERE T.transactionId = " + to_string(transactionId) + ";";

    double transactionAmount = 0;
    int participantCount = 0;
    char* errMsg = nullptr;

    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int {
        if (argc >= 2 && argv[0] && argv[1]) {
            double* ptr = static_cast<double*>(data);
            ptr[0] = atof(argv[0]);  
            ptr[1] = atoi(argv[1]);  
        }
        return 0;
    };

    double result[2] = {0, 0}; 
    int rc = sqlite3_exec(db, query.c_str(), callback, &result, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return -1;  
    }

    double amountPerParticipant = result[0] / (result[1] + 1); 
    return amountPerParticipant;
}
void DataBaseManager::updateRequiredAmount(int transactionId, double required) {
    const string updateQuery = "UPDATE Participant SET requiredAmount = " + to_string(required) +
                               " WHERE transactionId = " + to_string(transactionId) + ";";

    char* errMsg = nullptr;
    
    int rc = sqlite3_exec(db, updateQuery.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
         Logger::getInstance()->log(LogLevel::INFO,"", "Updated required amount for Shared Transaction ID: %d to %.2f.", 
                               transactionId, required);
    }
}
void DataBaseManager::updatePaidAmount(int transactionId, double paid) {
    const string selectQuery = "SELECT userId, paidAmount, requiredAmount FROM Participant "
                               "WHERE transactionId = " + to_string(transactionId) + " AND paidAmount > requiredAmount;";

    vector<int> participantIds;
    char* errMsg = nullptr;
    
    auto callback = [](void* data, int argc, char** argv, char** azColName) -> int {
        vector<int>* ids = static_cast<vector<int>*>(data);
        if (argc > 0 && argv[0]) {
            ids->push_back(atoi(argv[0])); 
        }
        return 0;
    };

    int rc = sqlite3_exec(db, selectQuery.c_str(), callback, &participantIds, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return;
    }

    int numParticipants = participantIds.size();
    
    if (numParticipants > 0) {
        double amountPerParticipant = paid / numParticipants;

        for (int userId : participantIds) {
            const string updateQuery = "UPDATE Participant SET paidAmount = paidAmount - " + to_string(amountPerParticipant) +
                                       " WHERE userId = " + to_string(userId) + " AND transactionId = " + to_string(transactionId) + ";";

            rc = sqlite3_exec(db, updateQuery.c_str(), nullptr, nullptr, &errMsg);
            if (rc != SQLITE_OK) {
                cerr << "SQL error in update: " << errMsg << endl;
                sqlite3_free(errMsg);
            } else {
                Logger::getInstance()->log(LogLevel::INFO, "", "Updated paid amount for UserID: %d in TransactionID: %d by %.2f", 
                                           userId, transactionId, amountPerParticipant);
            }
        }
    } 
}
void DataBaseManager::settleParticipantPayment(int transactionId, int userId, double newValue) {
    const string selectQuery = "SELECT paidAmount, requiredAmount FROM Participant "
                               "WHERE transactionId = " + to_string(transactionId) + 
                               " AND userId = " + to_string(userId) + ";";

    double paidAmount = 0.0;
    double requiredAmount = 0.0;
    char* errMsg = nullptr;

    auto callback = [](void* data, int argc, char** argv, char** azColName) -> int {
        if (argc > 1 && argv[0] && argv[1]) {
            double* amounts = static_cast<double*>(data);
            amounts[0] = atof(argv[0]); 
            amounts[1] = atof(argv[1]); 
        }
        return 0;
    };

    double amounts[2] = {0.0, 0.0}; 
    int rc = sqlite3_exec(db, selectQuery.c_str(), callback, amounts, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return;
    }

    paidAmount = amounts[0];
    requiredAmount = amounts[1];

    double totalPaid = paidAmount + newValue;

    if (totalPaid > requiredAmount) {
        totalPaid = requiredAmount;
    }

    const string updateQuery = "UPDATE Participant SET paidAmount = " + to_string(totalPaid) +
                               " WHERE transactionId = " + to_string(transactionId) +
                               " AND userId = " + to_string(userId) + ";";

    rc = sqlite3_exec(db, updateQuery.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error in update: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        Logger::getInstance()->log(LogLevel::INFO, "User Id :"+to_string(userId), "Updated paid amount in TransactionID: %d to %.2f", 
                                   transactionId, totalPaid);
    }
}



double DataBaseManager::calculateTotalIncoms(int userId) {
    const string incomeQuery = "SELECT SUM(amount) FROM \"Transaction\" "
                               "WHERE userId = " + to_string(userId) + " AND type = "+ to_string(static_cast<int>(TransactionType::DEPOSIT) )+ ";";

    char* errMsg = nullptr;
    double totalIncome = 0.0;

    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int {
        if (argc > 0 && argv[0]) {
            *(static_cast<double*>(data)) = atof(argv[0]); 
        }
        return 0;
    };

    int rc = sqlite3_exec(db, incomeQuery.c_str(), callback, &totalIncome, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    return totalIncome;
}
double DataBaseManager::calculateTotalExponses(int userId) {
    const string query = "SELECT SUM(amount) FROM \"Transaction\" WHERE userId = " + to_string(userId) + " AND type = "+ to_string(static_cast<int>(TransactionType::WITHDRAW) )+ ";";

    const string sharedQuery = "SELECT SUM(paidAmount) FROM Participant "
                               "JOIN \"Transaction\" ON Participant.transactionId = \"Transaction\".transactionId "
                               "WHERE Participant.userId = " + to_string(userId) + ";";

    double totalExpenses = 0.0;
    char* errMsg = nullptr;

    auto callback = [](void* data, int argc, char** argv, char** azColName) -> int {
        double* total = static_cast<double*>(data);
        if (argc > 0 && argv[0]) {
            *total += atof(argv[0]);
        }
        return 0;
    };

    int rc = sqlite3_exec(db, query.c_str(), callback, &totalExpenses, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    rc = sqlite3_exec(db, sharedQuery.c_str(), callback, &totalExpenses, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error in shared transactions: " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    return totalExpenses;
}
double DataBaseManager::calculateTotalExponses(int userId,Categories category) {
    const string query = "SELECT SUM(amount) FROM \"Transaction\" WHERE userId = " + to_string(userId) + " AND type = "+ to_string(static_cast<int>(TransactionType::WITHDRAW) )+" AND category = "+ to_string(static_cast<int>(category) )+ ";";

    const string sharedQuery = "SELECT SUM(paidAmount) FROM Participant "
                               "JOIN \"Transaction\" ON Participant.transactionId = \"Transaction\".transactionId "
                               "WHERE Participant.userId = " + to_string(userId) + " AND \"Transaction\".category = "+ to_string(static_cast<int>(category) )+";";

    double totalExpenses = 0.0;
    char* errMsg = nullptr;

    auto callback = [](void* data, int argc, char** argv, char** azColName) -> int {
        double* total = static_cast<double*>(data);
        if (argc > 0 && argv[0]) {
            *total += atof(argv[0]);
        }
        return 0;
    };

    int rc = sqlite3_exec(db, query.c_str(), callback, &totalExpenses, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    rc = sqlite3_exec(db, sharedQuery.c_str(), callback, &totalExpenses, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error in shared transactions: " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    return totalExpenses;
}


const char *DataBaseManager::toStringCategory(Categories category) const{
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
const char *DataBaseManager::toStringType(TransactionType type) const{
    switch (type)
    {
    case TransactionType::DEPOSIT :
        return "DEPOSIT";
    case TransactionType::WITHDRAW :
        return "WITHDRAW";
    default:
        return "";
    }
}

vector<vector<string>> DataBaseManager::executeQuery(const string& query) {
    vector<vector<string>> result;
    char* errorMessage = nullptr;
    
    auto callback = [](void* data, int argc, char** argv, char** colNames) -> int {
        vector<vector<string>>* result = static_cast<vector<vector<string>>*>(data);
        vector<string> row;
        for (int i = 0; i < argc; i++) {
            row.push_back(argv[i] ? argv[i] : "NULL");
        }
        result->push_back(row);
        return 0;
    };

    int rc = sqlite3_exec(db, query.c_str(), callback, &result, &errorMessage);
    
    if (rc != SQLITE_OK) {
        cout << "SQL error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
    
    return result;
}
void DataBaseManager::generateTransactionReport(const Date &d1, const Date &d2, int userId) {
     string query = 
        "SELECT transactionId, amount, category, type, date FROM \"Transaction\" "
        "WHERE userId = " + to_string(userId) +
        " AND date BETWEEN '" + d1.toString() + "' AND '" + d2.toString() + "' ;";

    vector<vector<string>> result = executeQuery(query);

    cout << "TransactionId | Amount | Category | Type | Date\n";
    cout << "-------------------------------------------------\n"; 

    for (const auto& row : result) {
        cout << row[0] << " | " << row[1] << " | " << toStringCategory(static_cast<Categories>(stoi(row[2]))) << " | " 
             << toStringCategory(static_cast<Categories>(stoi(row[3]))) << " | " << row[4] << "\n";
    }
}
void DataBaseManager::genarateSharedTransactionReport(const Date &d1, const Date &d2, int userId) {
    string query = 
        "SELECT ST.transactionId, ST.amount, ST.date, P.paidAmount, P.requiredAmount "
        "FROM \"Transaction\" ST JOIN Participant P ON ST.transactionId == P.transactionId "
        "WHERE P.userId = " + to_string(userId) +
        " AND ST.date BETWEEN '" + d1.toString() + "' AND '" + d2.toString() + "';";
    
    vector<vector<string>> result = executeQuery(query);

    cout << "TransactionId | Amount | Date | PaidAmount | RequiredAmount\n";
    cout << "----------------------------------------------------------\n";

    for (const auto& row : result) {
        cout << row[0] << " | " << row[1] << " | " << row[2] << " | " 
             << row[3] << " | " << row[4] << "\n";
    }
}
void DataBaseManager::genarateSavingPlansReport(const Date &d1, const Date &d2, int userId) {
    string query = 
        "SELECT planId, savingAmount, target, startDate, endDate "
        "FROM SavingPlan "
        "WHERE userId = " + to_string(userId) + 
        " AND startDate BETWEEN '" + d1.toString() + "' AND '" + d2.toString() + "';";

    vector<vector<string>> result = executeQuery(query);

    cout << "PlanId | SavingAmount | Target | StartDate | EndDate\n";
    cout << "------------------------------------------------------\n";

    for (const auto& row : result) {
        cout << row[0] << " | " << row[1] << " | " << row[2] << " | " 
             << row[3] << " | " << row[4] << "\n";
    }
}