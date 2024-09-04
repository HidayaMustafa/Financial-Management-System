#include "SharedTransaction.hpp"
#include "User.hpp"

SharedTransaction::SharedTransaction(int id, double amount, Categories category, const User &initialParticipant)
    : Transaction(id, amount, category, TransactionType::WITHDRAW)
{
    this->setIsShared(true);
    Logger::getInstance()->log(Info, initialParticipant.getName(),"Shared Transaction ID: %d created successfully", this->getTranId());
    addParticipant(initialParticipant, amount);
}

SharedTransaction::~SharedTransaction(){
    for (Participant *p : participants){
        if (p != nullptr) {
            delete p; 
        }
    }
    participants.clear();
}

void SharedTransaction::addParticipant(const User &u, double paid)
{
    for (const auto &participant : participants) {
        if (participant->getUser().getId() == u.getId()) {
            Logger::getInstance()->log(Error, u.getName(), "%s is already in the Shared Transaction ID: %d.", u.getName(),this->getTranId());
            return;
        }
    }
    if (!u.checkBudget(this->getCategory(), paid)) {
        Logger::getInstance()->log(Error, u.getName(), "Can't add %s in the Shared Transaction ID:%d  because the budget for %s exceeds.",u.getName().c_str(),this->getTranId(), u.printCategory(this->getCategory()));
        return;
    }
    if (u.calculateTotal() >= paid) {
        double required = CalculateParticipantAmountRequired();
        if (paid <= required) {
            Participant *participant = new Participant(u, paid,required);
            participants.push_back(participant);
            UpdateRequiredAmount(u.getName(),required);
            u.joinSharedTransaction(*this);
            int count=0;
            for (auto &participant : participants) {
                if(participant->getPaid() > required){
                    count++;
                }
            }
            int p=paid/count;
            for (auto &participant : participants) {
                if(participant->getPaid() > required){
                    participant->setPaid(participant->getPaid()-p);
                }
            }
            Logger::getInstance()->log(Info, u.getName(), " %s Added successfully to Shared Transaction ID %d -- %.2f $ withdrawn successfully from %s.", u.getName(), this->getTranId(), paid, u.printCategory(this->getCategory()));
            Logger::getInstance()->log(Info, u.getName(), " Updated Required amount to %.2f for SharedTransaction ID: %d.", required,this->getTranId());
            if (u.isWarnBudget(this->getCategory())) {
                Logger::getInstance()->log(Warn, u.getName(), "The budget in %s has decreased to below 80%% of its previous amount.", u.printCategory(this->getCategory()));
            }
        } else {
            Logger::getInstance()->log(Warn, u.getName(), " Can't pay a value more than required: %.2f", required);
        }
    } else {
        Logger::getInstance()->log(Error, u.getName(), "Subscribe in Shared Transaction of %.2f $ failed. Exceeds total balance.", paid);
    }
}

double SharedTransaction:: CalculateParticipantAmountRequired() {
        return this->getAmount() / (participants.size()+1);
}
void SharedTransaction::UpdateRequiredAmount(string name ,double required){
    for (auto &participant : participants) {
        participant->setRequired(required);
    }
    Logger::getInstance()->log(Error, name, "Required amount in Shared Transaction ID: %d $ updated to %s.",this->getTranId(), required);
}

void SharedTransaction::updateParticipantPaid(const User &u, double newValue)
{
    for (auto &participant : participants) {
        if (participant->getUser().getId() == u.getId()) {
            if (u.calculateTotal() >= newValue && u.checkBudget(this->getCategory(), newValue)) {
                if(newValue + participant->getPaid() > participant->getRequired()){
                    participant->updatePaidValue(newValue);
                    Logger::getInstance()->log(Info, u.getName(), " Updated payment for %s to %.2f in Shared Transaction ID %d", u.getName(),participant->getPaid() , this->getTranId());
                }else{
                    Logger::getInstance()->log(Warn, u.getName(), " Can't pay a value more than required: %.2f", participant->getRequired());
                }  
            } else {
                Logger::getInstance()->log(Error, u.getName(), "Failed to update payment for %s: conditions not met in Shared Transaction ID %d", u.getName(), this->getTranId());
            }
            return;
        }
    }
    Logger::getInstance()->log(Error, u.getName(), "Participant %s not found in Shared Transaction ID %d", u.getName(), this->getTranId());
}

void SharedTransaction::removeParticipant(User &u)
{
    auto it = std::find_if(participants.begin(), participants.end(), [&u](Participant *participant)
                           { return participant->getUser().getId() == u.getId(); });
    if (it != participants.end()) {
        delete (*it);
        participants.erase(it);
        double required = CalculateParticipantAmountRequired();
        UpdateRequiredAmount(u.getName(),required);
        Logger::getInstance()->log(Info, u.getName(), "Participant %s removed from Shared Transaction ID %d", u.getName(), this->getTranId());
        u.deleteSharedTransaction(*this);
    } else {
        Logger::getInstance()->log(Error, u.getName(), "Failed to remove participant %s: not found in Shared Transaction ID %d", u.getName(), this->getTranId());
    }
}

Participant *SharedTransaction::getParticipant(const User &u)
{
    for (const auto &participant : participants) {
        if (u.getId() == participant->getUser().getId()) {
            return participant;
        }
    }
    Logger::getInstance()->log(Error, u.getName(), "Participant %s not found in Shared Transaction ID %d", u.getName(), this->getTranId());
    return nullptr;
}

std::vector <Participant*>SharedTransaction::getParticipants()const
{
     return participants;
}

void SharedTransaction::printParticipants() const
{
    std::cout << "-> Participants for SharedTransaction ID " << getTranId() << ":\n";
    for (const auto &participant : participants)
    {
        std::cout << "*- " << participant->getUser().getName()
                  << " --> Paid: " << participant->getPaid()
                  << " || Required: " << participant->getRequired() << '\n';
    }
    std::cout << "----------------------\n";
}

