#pragma once

#include <iostream>
#include <string>
#include "Date.hpp"  
#include "Logger.hpp"
#include "DataBaseManager.hpp"

class SavingPlan {
private:
    int id;
    double savingAmount;
    double target;
    Date startDate;
    Date endDate;
    std::string name;

public:
    SavingPlan(double newtarget, Date newStartDate, Date newEndDate, std::string Name);

    ~SavingPlan();

    int getId() const;
    void setId(int ID);
    double getSavingAmount() const;
    void setStartDate(const Date& d1);
    void setEndDate(const Date& d1);
    void setTarget(double t);
    void checkSavingPlan() const;
    double getTarget() const;
    Date getStartDate() ;
    Date getEndDate();
};
