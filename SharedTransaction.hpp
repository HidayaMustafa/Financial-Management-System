#pragma once
#include <iostream>
#include <vector>
#include "Transaction.hpp"
#include "Participant.hpp"
#include "User.hpp"
#include "Logger.hpp"

class Participant;
class User;

class SharedTransaction : public Transaction {
private:
    std::vector<Participant*> participants;
public:
    SharedTransaction(int id, double amount, Categories category,const User& initialParticipant);
    virtual ~ SharedTransaction()override;
    void addParticipant(const User& user, double paid);
    double CalculateParticipantAmountRequired();
    void UpdateRequiredAmount(string userName ,double required);

    void updateParticipantPaid(const User& user, double newValue);

    void removeParticipant(User& user);

    void printParticipants() const;

    Participant* getParticipant(const User& user);
    
    std::vector <Participant*>getParticipants()const;

    bool getIsShared() override;
};
