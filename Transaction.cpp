#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

using namespace std;

enum Categories {
    Food ,
    Clothes ,
    Shoes ,
    Electronics ,
    Miscellaneous ,
    null
};

enum TransactionType {
    Deposit,
    Withdraw
};

class Date {
    private:
        int day;
        int month;
        int year;
    
    public:
        Date() {
            auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
            tm *localTime = localtime(&now);

            day = localTime->tm_mday;
            month = localTime->tm_mon + 1;
            year = localTime->tm_year + 1900; 
        }

        Date(int d, int m, int y) : day(d), month(m), year(y) {}


        bool operator>=(Date &d) {
        if (year > d.year) return true;
        if (year == d.year && month > d.month) return true;
        if (year == d.year && month == d.month && day >= d.day) return true;
        return false;
        }

        bool operator<=(Date & d) {
        if (year < d.year) return true;
        if (year == d.year && month < d.month) return true;
        if (year == d.year && month == d.month && day <= d.day) return true;
        return false;
        }

        int operator-(Date & d) {
            tm tm1 = {};
            tm1.tm_year = year;
            tm1.tm_mon = month;
            tm1.tm_mday = day;

            tm tm2 = {};
            tm2.tm_year = d.year;
            tm2.tm_mon = d.month;
            tm2.tm_mday = d.day;

            time_t time1 = std::mktime(&tm1);
            time_t time2 = std::mktime(&tm2);

            double diff = difftime(time1, time2) / (60 * 60 * 24);
            
            return static_cast<int>(diff);
        }

        void display() {
            cout << setw(2) << setfill('0') << day << '/'
                 << setw(2) << setfill('0') << month << '/'
                 << year;
        }
};

class Transaction {
    private:
        int id;
        double amount;
        enum Categories category;
        enum TransactionType type;
        Date date;

    public:
        Transaction(int Id , double num, Categories cat, TransactionType t) 
            : id(Id) , amount(num), category(cat), type(t), date() {}

        double getAmount()  {
            return amount;
        }

        void setAmount(double amt){
            amount = amt;
        }

        int getId(){
            return id;
        }

        Categories getCategory()  {
            return category;
        }

        void setCategory(Categories cat){
            category = cat;
        }

        Date getDate(){
            return date;
        }

        void updateDate(){
            date=Date();
        }

        TransactionType getTransactionType(){
            return type;
        }

        void setTransactionType(TransactionType t){
            type=t;
        }

};




