#include "SavingPlan.hpp"
#include "Logger.hpp"

SavingPlan::SavingPlan(double newTarget, Date newStartDate, Date newEndDate)
    : savingAmount(0), target(newTarget), startDate(newStartDate), endDate(newEndDate) {
    }

SavingPlan::~SavingPlan() {
}

int SavingPlan::getId() const {
    return id;
}

void SavingPlan::setId(int ID) {
    id = ID;
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

Date SavingPlan::getStartDate() {
    return startDate;
}
Date SavingPlan::getEndDate() {
    return endDate;
}

void SavingPlan::setTarget(double t) {
    target = t;
}

double SavingPlan::getTarget() const {
    return target;
}

