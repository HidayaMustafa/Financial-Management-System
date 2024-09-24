#include "SharedTransaction.hpp"

SharedTransaction::SharedTransaction(double amount, Categories category, const User &initialParticipant)
    : Transaction(amount, category, TransactionType::WITHDRAW)
{
    DataBaseManager::getInstance().insertTransaction(0, *this);
    addParticipant(initialParticipant, amount);
}

SharedTransaction::~SharedTransaction(){
}

void SharedTransaction::addParticipant(const User &user, double paid) {
    if (user.isBudgetExceeded(this->getCategory(), paid)) {
        Logger::getInstance()->log(LogLevel::ERROR, "User Id: "+to_string(user.getUserId()), 
                                   "Participant cannot be added to Shared Transaction ID: %d. Budget exceeded for category %s.",
                                   this->getTranId(), user.printCategory(this->getCategory()));
        return;
    }

    if (user.calculateCurrentBalance() >= paid) {
        double required = calculateParticipantAmountRequired();
        if (paid <= required) {
            Participant participant(user, paid, required);
            DataBaseManager::getInstance().insertParticipant(participant.getUser().getUserId(), this->getTranId(), participant);

            updateRequiredAmount(required);
            updatePaidAmount(paid);

            } else {
            Logger::getInstance()->log(LogLevel::ERROR, "User Id: "+to_string(user.getUserId()), 
                                       "Cannot pay more than the required amount in Shared Transaction ID: %d. Paid: %.2f, Required: %.2f.",
                                       this->getTranId(), paid, required);
        }
    } else {
        Logger::getInstance()->log(LogLevel::ERROR, "User Id: "+to_string(user.getUserId()), 
                                   "Shared Transaction ID: %d failed for %.2f $. Insufficient balance.",
                                   this->getTranId(), paid);
    }
}

double SharedTransaction::calculateParticipantAmountRequired() {
    double required = DataBaseManager::getInstance().calculateParticipantAmountRequired(this->getTranId());
    return required;
}

void SharedTransaction::updateRequiredAmount(double required) {
    DataBaseManager::getInstance().updateRequiredAmount(this->getTranId(), required);
}

void SharedTransaction::updatePaidAmount(double paid) {
    DataBaseManager::getInstance().updatePaidAmount(this->getTranId(), paid);
    Logger::getInstance()->log(LogLevel::INFO,"", "Updated paid amount for Shared Transaction ID: %d to %.2f.", 
                               this->getTranId(), paid);
}

void SharedTransaction::settleParticipantPayment(const User &user, double newValue) {
    if (user.calculateCurrentBalance() >= newValue && !user.isBudgetExceeded(this->getCategory(), newValue)) {
        DataBaseManager::getInstance().settleParticipantPayment(this->getTranId(), user.getUserId(), newValue);
        updatePaidAmount(newValue);
    } else {
        Logger::getInstance()->log(LogLevel::ERROR, "User Id: "+to_string(user.getUserId()), 
                                   "Failed to settle payment in Shared Transaction ID: %d. Insufficient balance or budget exceeded.",
                                   this->getTranId());
    }
}

void SharedTransaction::deleteSharedTransaction() {
    DataBaseManager::getInstance().deleteTransaction(0,this->getTranId());
}
