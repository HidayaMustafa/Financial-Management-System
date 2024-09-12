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

void SharedTransaction::addParticipant(const User &u, double paid){
    Participant *participant = new Participant(u, paid,0);
    DataBaseManager::getInstance().insertParticipant(participant->getUser().getUserId(),this->getTranId(),*participant);         
}

double SharedTransaction:: CalculateParticipantAmountRequired() {
        return 0.0;
}
void SharedTransaction::UpdateRequiredAmount(string name ,double required){
}

void SharedTransaction::updateParticipantPaid(const User &u, double newValue){
}

void SharedTransaction::removeParticipant(User &u)
{
}

void SharedTransaction::deleteSharedTransaction(){
    DataBaseManager::getInstance().deleteTransaction(this->getTranId());
}