#include "SavingPlan.hpp"

SavingPlan::SavingPlan(int Id, double newtarget, Date newStartDate, Date newEndDate, std::string Name)
    : id(Id), savingAmount(0), target(newtarget), startDate(newStartDate), endDate(newEndDate), name(Name) {
    Logger::getInstance()->log(INFO, name, "Saving plan %d created.", id);
}

SavingPlan::~SavingPlan() {
    Logger::getInstance()->log(INFO, name, "Saving plan %d deleted.", id);
}

int SavingPlan::getPlanId() const {
    return id;
}

double SavingPlan::getSavingAmount() const {
    return savingAmount;
}

void SavingPlan::setStartDate(const Date& d1) {
    startDate = d1;
}

void SavingPlan::setEndDate(const Date& d1) {
    endDate = d1;
}

void SavingPlan::setTarget(double t) {
    target = t;
}

void SavingPlan::transferAmount(double amount) {
    savingAmount += amount;
    Logger::getInstance()->log(INFO, name, "%.2f$ added successfully to saving plan %d.", amount, id);
}

void SavingPlan::checkSavingPlan() const {
    double amountToSave = target - savingAmount;

    if (amountToSave <= 0) {
        Logger::getInstance()->log(INFO, name, "Good job, You have met your saving target in %d days.", (endDate - startDate));
        return;
    }

    Date currentDate = Date();
    int remainingDays = endDate - currentDate;
    if (remainingDays <= 0) {
        Logger::getInstance()->log(ERROR, name, "You didn't achieve your savings target within the time limit.");
    } else {
        Logger::getInstance()->log(WARN, name, "You still need to save %.2f$ to meet your target. %d days remaining.", amountToSave, remainingDays);
    }
}
