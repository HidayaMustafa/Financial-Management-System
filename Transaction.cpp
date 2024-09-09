#include "Transaction.hpp"

Transaction::Transaction(int Id, double num, Categories cat, TransactionType t)
    : id(Id), amount(num), category(cat), type(t), date(){}

Transaction::~Transaction(){
    
}
double Transaction::getAmount() const {
    return amount;
}

void Transaction::setAmount(double amt) {
    amount = amt;
}

bool Transaction::getIsShared(){
    return false;
}

int Transaction::getTranId() const {
    return id;
}

Categories Transaction::getCategory() const {
    return category;
}

void Transaction::setCategory(Categories cat) {
    category = cat;
}

Date Transaction::getDate() const {
    return date;
}

void Transaction::updateDate() {
    date = Date();
}

TransactionType Transaction::getTransactionType() const {
    return type;
}

void Transaction::setTransactionType(TransactionType t) {
    type = t;
}
