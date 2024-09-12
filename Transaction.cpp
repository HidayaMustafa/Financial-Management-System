#include "Transaction.hpp"

Transaction::Transaction(double num, Categories cat, TransactionType t)
    : amount(num), category(cat), type(t), date(){}

Transaction::~Transaction(){
    
}
double Transaction::getAmount() const {
    return amount;
}

void Transaction::setAmount(double amt) {
    amount = amt;
}

int Transaction::getTranId() const {
    return id;
}
void Transaction::setTranId(int ID) {
    id= ID;
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

const char *Transaction::printCategory(Categories category) const
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

const char *Transaction::printType(TransactionType type) const
{
    switch (type)
    {
    case TransactionType::DEPOSIT:
        return "DEPOSIT";
    case TransactionType::WITHDRAW:
        return "WITHDRAW";
    default:
        return "NONE";
    }
}
