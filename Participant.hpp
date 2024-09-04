#pragma once

#include "User.hpp"  
class User;

class Participant {
private:
    const User& user;
    double paidValue;
    double requiredValue;
public:
    Participant(const User& u, double paid,double required);
    const User& getUser() const;
    void updatePaidValue(double newValue);
    double getPaid()const;
    void setPaid(double value);
    double getRequired()const;
    void setRequired(double value);
};
