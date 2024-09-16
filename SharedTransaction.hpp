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
    void addParticipant(const User& user, double paid);
    double calculateParticipantAmountRequired();
    void updateRequiredAmount(double required);
    void updatePaidAmount(double paid);

    void settleParticipantPayment(const User& user, double newValue);

    void deleteSharedTransaction();
};
