#pragma once
#include <iostream>
#include <vector>
#include "Transaction.hpp"
#include "Participant.hpp"
#include "User.hpp"
#include "Logger.hpp"
#include "DataBaseManager.hpp"

class Participant;
class User;


class SharedTransaction : public Transaction { 
public:
    SharedTransaction(double amount, Categories category,const User& initialParticipant);
    virtual ~ SharedTransaction()override;
    void addParticipant(const User& u, double paid);
    double CalculateParticipantAmountRequired();
    void UpdateRequiredAmount(string name ,double required);

    void updateParticipantPaid(const User& u, double newValue);

    void removeParticipant(User& u);
    void deleteSharedTransaction();
};
