#include "SharedTransaction.hpp"
#include "User.hpp"

SharedTransaction::SharedTransaction(int id, double amount, Categories category, const User& initialParticipant, double required)
    : Transaction(id, amount, category, TransactionType::WITHDRAW)
{
    addParticipant(initialParticipant, 0.0, required);
}

void SharedTransaction::addParticipant(const User& u, double paid, double required) {
    for (const auto& participant : participants) {
        if (participant->getUser().getId() == u.getId()) {
            return; 
        }
    }
    if (!u.checkBudget(this->getCategory(), paid))
    {
        //Logger::getInstance()->log(Error, name, "Withdraw of %.2f $ failed from %s. Exceeds budget.", value, printCategory(category));
        return;
    }
    if (u.calculateTotal() >= paid)
    {
        if(paid <= required){
        Participant* participant = new Participant(u, paid, required);
        participants.push_back(participant);
        u.joinSharedTransaction(*this);
        //Logger::getInstance()->log(Info, name, " Transaction ID %d -- %.2f $ withdrawn successfully from %s.", transId, value, printCategory(category));
        if (u.isWarnBudget(this->getCategory()))
        {
            //Logger::getInstance()->log(Warn, name, " The budget in %s has decreased to below 80%% of its previous amount.", printCategory(category));
        }
        }else{
            // -- add error 
        }
    }
    else
    {
        //Logger::getInstance()->log(Error, name, "Withdraw of %.2f $ failed. Exceeds total balance.", value);
    }
}

void SharedTransaction::updateParticipantPaid(const User& u, double newValue) {
    for (auto& participant : participants) {
        if (participant->getUser().getId() == u.getId()) {
            if(newValue > participant->getPaid() && u.calculateTotal()>= newValue && u.checkBudget(this->getCategory(),newValue)){
                participant->updatePaidValue(newValue);
                return;
            }else{
                // add error (not meet the conditions)
            }
        }
    }
    // add error (not found)
}

void SharedTransaction::exitParticipant(const User& u) {
    auto it = std::find_if(participants.begin(), participants.end(), [&u](Participant* participant) {
        return participant->getUser().getId() == u.getId();
    });
    if (it != participants.end()) {
        participants.erase(it);
        const_cast<User&>(u).deleteSharedTransaction(*this);
    }
}

void SharedTransaction::printParticipants() const{
    std::cout << "-> Participants for SharedTransaction ID " << getTranId() << ":\n";
    for (const auto& participant : participants) {
        std::cout << "*- " << participant->getUser().getName()
                  << " || Paid: " << participant->getPaid()
                  << " || Required: " << participant->getRequired() << '\n';
    }
    std::cout << "----------------------\n";
}

Participant* SharedTransaction::getParticipant(const User& u) {
    for (const auto& participant : participants) {
        if (u.getId() == participant->getUser().getId()) {
            return participant; 
        }
    }
    return nullptr; 
}

