#include <iostream>
#include <string>

class SavingPlan
{
private:
    int id;
    double savingAmount;
    double target;
    Date startDate;
    Date endDate;
    string name;

public:
    SavingPlan(int Id, double newtarget, Date newStartDate, Date newEndDate,string Name) : id(Id), target(newtarget), startDate(newStartDate), endDate(newEndDate), savingAmount(0)
    {
        name=Name;
        Logger::getInstance()->log(Info, name, " Saving plan %d created.", id);
    }

    ~SavingPlan(){
       Logger::getInstance()->log(Info,name," Saving plane %d deleted ",id);
    }

    int getId()
    {
        return id;
    }

    double getSavingAmount()
    {
        return savingAmount;
    }

    void setStartDate(Date d1)
    {
        startDate = d1;
    }

    void setEndDate(Date d1)
    {
        endDate = d1;
    }

    void setTarget(double t)
    {
        target = t;
    }

    void transferAmount(double amount)
    {
        savingAmount += amount;
        Logger::getInstance()->log(Info, name, " %.2f $ added successfully to saving plan %d.", amount, id);
    }

    void checkSavingPlan()
    {
        double amountToSave = target - savingAmount;

        if (amountToSave <= 0)
        {
            Logger::getInstance()->log(Info, name, " Good job, You have met your saving target in the %d days.", (endDate - startDate));
            return;
        }
        Date currentDate = Date();
        int remainingDays = endDate - currentDate;
        if (remainingDays <= 0)
        {
            Logger::getInstance()->log(Error, name, "You didn't achieve your savings target within the limit time.");
        }
        else
        {
            Logger::getInstance()->log(Warn, name, " You still need to save %.2f$ to meet your target. %d days remaining.", amountToSave, remainingDays);
        }
    }
};