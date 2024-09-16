#include "SharedTransaction.hpp"

SharedTransaction::SharedTransaction(double amount, Categories category, const User &initialParticipant)
    : Transaction(amount, category, TransactionType::WITHDRAW)
{
    DataBaseManager::getInstance().insertTransaction(0,*this);
    Logger::getInstance()->log(Info, initialParticipant.getUserName(),"Shared Transaction ID: %d created successfully", this->getTranId());
    addParticipant(initialParticipant, amount);
}

SharedTransaction::~SharedTransaction(){
}

void SharedTransaction::addParticipant(const User &user, double paid){
    // if (DataBaseManager::getInstance().findParticipant(user,this.getTranId())) {
    //    // Logger::getInstance()->log(Error, u.getName(), "%s is already in the Shared Transaction ID: %d.", u.getName().c_str(),this->getTranId());
    //     return;
    // }
    if (user.isBudgetExceeded(this->getCategory(), paid)) {
        return;
    }
    if (user.calculateCurrentBalance() >= paid) {
        double required = calculateParticipantAmountRequired();
        if (paid <= required) {
            Participant participant (user, paid,required);
            DataBaseManager::getInstance().insertParticipant(participant.getUser().getUserId(),this->getTranId(),participant);   

            updateRequiredAmount(required);
            updatePaidAmount(paid);

        } else {
            //Logger::getInstance()->log(Warn, user.getName(), " Can't pay a value more than required: %.2f", required);
        }
    } else {
        //Logger::getInstance()->log(Error, user.getName(), "Subscribe in Shared Transaction of %.2f $ failed. Exceeds total balance.", paid);
    }      
}

double SharedTransaction:: calculateParticipantAmountRequired() {
    return DataBaseManager::getInstance().calculateParticipantAmountRequired(this->getTranId());
}
void SharedTransaction::updateRequiredAmount(double required){
    DataBaseManager::getInstance().updateRequiredAmount(this->getTranId(),required);
}
void SharedTransaction::updatePaidAmount(double paid){
    DataBaseManager::getInstance().updatePaidAmount(this->getTranId(),paid);
}

void SharedTransaction::settleParticipantPayment(const User &user, double newValue){
    if (user.calculateCurrentBalance() >= newValue && ! user.isBudgetExceeded(this->getCategory(), newValue)) {
        DataBaseManager::getInstance().settleParticipantPayment(this->getTranId(),user.getUserId(),newValue);
        updatePaidAmount(newValue);
    }else{
        //---- add error
    }
}

void SharedTransaction::deleteSharedTransaction(){
    DataBaseManager::getInstance().deleteTransaction(this->getTranId());
}