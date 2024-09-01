#include "Participant.hpp"

Participant::Participant(const User& u, double paid, double required)
    : user(u), paidValue(paid), requiredValue(required) {}

const User& Participant::getUser() const {
    return user;
}

void Participant::updatePaidValue(double newValue){
    if (paidValue + newValue <= requiredValue) {
        paidValue += newValue;
    } else {
        // Logger::getInstance()->log(LogLevel::Error, user.getName(), "Can't add value more than required");
    }
}

double Participant::getPaid()const {
    return paidValue;
}
double Participant::getRequired()const {
    return requiredValue;
}
