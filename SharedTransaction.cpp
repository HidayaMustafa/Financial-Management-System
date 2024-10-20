#include "SharedTransaction.hpp"
#include "User.hpp"

SharedTransaction::SharedTransaction(int id, double amount, Categories category, const User &initialParticipant)
    : Transaction(id, amount, category, TransactionType::WITHDRAW){
    Logger::getInstance()->log(INFO, initialParticipant.getUserName(),"Shared Transaction ID: %d created successfully", this->getTranId());
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

bool SharedTransaction::getIsShared(){
    return true;
}

void SharedTransaction::addParticipant(const User &user, double paid){
    for (const auto &participant : participants) {
        if (participant->getUser().getUserId() == user.getUserId()) {
            Logger::getInstance()->log(ERROR, user.getUserName(), "%s is already in the Shared Transaction ID: %d.", user.getUserName().c_str(),this->getTranId());
            return;
        }
    }
    if (!user.checkBudget(this->getCategory(), paid)) {
        Logger::getInstance()->log(ERROR, user.getUserName(), "Can't add %s in the Shared Transaction ID:%d  because the budget for %s exceeds.",user.getUserName().c_str(),this->getTranId(), user.printCategory(this->getCategory()));
        return;
    }
    if (user.calculateTotal() >= paid) {
        double required = CalculateParticipantAmountRequired();
        if (paid <= required) {
            Participant *participant = new Participant(u, paid,required);
            participants.push_back(participant);
            UpdateRequiredAmount(user.getUserName(),required);
            user.joinSharedTransaction(this);
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
            Logger::getInstance()->log(INFO, user.getUserName(), " %s Added successfully to Shared Transaction ID %d -- %.2f $ withdrawn successfully from %s.", user.getUserName().c_str(), this->getTranId(), paid, user.printCategory(this->getCategory()));
            Logger::getInstance()->log(INFO, user.getUserName(), " Updated Required amount to %.2f for SharedTransaction ID: %d.", required,this->getTranId());
            if (user.isWarnBudget(this->getCategory())) {
                Logger::getInstance()->log(WARN, user.getUserName(), "The budget in %s has decreased to below 80%% of its previous amount.", user.printCategory(this->getCategory()));
            }
        } else {
            Logger::getInstance()->log(WARN, user.getUserName(), " Can't pay a value more than required: %.2f", required);
        }
    } else {
        Logger::getInstance()->log(ERROR, user.getUserName(), "Subscribe in Shared Transaction of %.2f $ failed. Exceeds total balance.", paid);
    }
}

double SharedTransaction:: CalculateParticipantAmountRequired() {
        return this->getAmount() / (participants.size()+1);
}
void SharedTransaction::UpdateRequiredAmount(string userName ,double required){
    for (auto &participant : participants) {
        participant->setRequired(required);
    }
    Logger::getInstance()->log(ERROR, userName, "Required amount in Shared Transaction ID: %d $ updated to %s.",this->getTranId(), required);
}

void SharedTransaction::updateParticipantPaid(const User &user, double newValue)
{
    for (auto &participant : participants) {
        if (participant->getUser().getUserId() == user.getUserId()) {
            if (user.calculateTotal() >= newValue && user.checkBudget(this->getCategory(), newValue)) {
                if(newValue + participant->getPaid() > participant->getRequired()){
                    participant->updatePaidValue(newValue);
                    Logger::getInstance()->log(INFO, user.getUserName(), " Updated payment for %s to %.2f in Shared Transaction ID %d", user.getUserName(),participant->getPaid() , this->getTranId());
                }else{
                    Logger::getInstance()->log(WARN, user.getUserName(), " Can't pay a value more than required: %.2f", participant->getRequired());
                }  
            } else {
                Logger::getInstance()->log(ERROR, user.getUserName(), "Failed to update payment for %s: conditions not met in Shared Transaction ID %d", user.getUserName(), this->getTranId());
            }
            return;
        }
    }
    Logger::getInstance()->log(ERROR, user.getUserName(), "Participant %s not found in Shared Transaction ID %d", user.getUserName(), this->getTranId());
}

void SharedTransaction::removeParticipant(User &user)
{
    auto it = std::find_if(participants.begin(), participants.end(), [&user](Participant *participant)
                           { return participant->getUser().getUserId() == user.getUserId(); });
    if (it != participants.end()) {
        delete (*it);
        participants.erase(it);
        double required = CalculateParticipantAmountRequired();
        UpdateRequiredAmount(user.getUserName(),required);
        Logger::getInstance()->log(INFO, user.getUserName(), "Participant %s removed from Shared Transaction ID %d", user.getUserName(), this->getTranId());
        user.deleteSharedTransaction(*this);
    } else {
        Logger::getInstance()->log(ERROR, user.getUserName(), "Failed to remove participant %s: not found in Shared Transaction ID %d", user.getUserName(), this->getTranId());
    }
}

Participant *SharedTransaction::getParticipant(const User &user ){
    for (const auto &participant : participants) {
        if (user.getUserId() == participant->getUser().getUserId()) {
            return participant;
        }
    }
    Logger::getInstance()->log(ERROR, user.getUserName(), "Participant %s not found in Shared Transaction ID %d", user.getUserName(), this->getTranId());
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
        std::cout << "*- " << participant->getUser().getUserName()
                  << " --> Paid: " << participant->getPaid()
                  << " || Required: " << participant->getRequired() << '\n';
    }
    std::cout << "----------------------\n";
}

