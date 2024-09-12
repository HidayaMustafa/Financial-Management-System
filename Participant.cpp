#include "Participant.hpp"

Participant::Participant(const User& user, double paid, double required)
    : user(user), paidValue(paid), requiredValue(required) {}

const User& Participant::getUser() const {
    return user;
}

void Participant::updatePaidValue(double newValue){
        paidValue += newValue;
}

double Participant::getPaid()const {
    return paidValue;
}
void Participant::setPaid(double value){
    paidValue = value;
}

double Participant::getRequired()const {
    return requiredValue;
}
void Participant::setRequired(double newRequired ) {
    requiredValue = newRequired;
}
