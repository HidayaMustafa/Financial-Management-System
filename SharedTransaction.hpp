#pragma once
#include <iostream>
#include <vector>
#include "Transaction.hpp"
#include "Participant.hpp"
#include "User.hpp"

class Participant;
class User;

class SharedTransaction : public Transaction {
private:
    std::vector<Participant*> participants;
public:
    SharedTransaction(int id, double amount, Categories category,const User& initialParticipant, double required);

    void addParticipant(const User& u, double paid, double required);

    void updateParticipantPaid(const User& u, double newValue);

    void exitParticipant(const User& u);

    void printParticipants() const;

    Participant* getParticipant(const User& u);
};
