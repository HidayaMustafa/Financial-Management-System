#include "SharedTransaction.hpp"

#include "User.hpp"
#include "Logger.hpp"


SharedTransaction::SharedTransaction(int id, double amount, Categories category)
    : Transaction(id, amount, category, TransactionType::WITHDRAW) {}

void SharedTransaction::addParticipant(User& u, double paid, double required) {
    participants.push_back(Participant(u, paid, required));
    u.getSharedTransaction().push_back(*this);
}

void SharedTransaction::updateParticipantPaid(const User& u, double newValue) {
    for (auto& par : participants) {
        if (u.getId() == par.getUser().getId()) {
            par.updatePaidValue(newValue);
            break;
        }
    }
}

void SharedTransaction::exitParticipant(const User& u) {
    for (auto it = participants.begin(); it != participants.end(); ++it) {
        if (u.getId() == it->getUser().getId()) {
            //participants.erase(it);
            break;
        }
    }
}

void SharedTransaction::printParticipants() {
    std::cout << "Participants for SharedTransaction ID " << ":\n";
    for (auto& participant : participants) {
        std::cout << "User ID: " << participant.getUser().getId()
                  << ", Paid: " << participant.getPaid() << '\n';
    }
    std::cout << "----------------------\n";
}
