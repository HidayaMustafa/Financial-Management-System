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
    ~ SharedTransaction()override;
    void addParticipant(const User& u, double paid);
    double CalculateParticipantAmountRequired();
    void UpdateRequiredAmount(string name ,double required);

    void updateParticipantPaid(const User& u, double newValue);

    void removeParticipant(User& u);

    void printParticipants() const;

    Participant* getParticipant(const User& u);
    std::vector <Participant*>getParticipants()const;
};
