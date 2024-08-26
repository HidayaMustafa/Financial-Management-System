#include <string>
#include "Wallet.cpp"
#include <vector>
#include "SavingPlan.cpp"
#include <algorithm> 

class User
{
private:
    string name;
    int id;
    Wallet wallet;
    vector<unique_ptr<SavingPlan>> plans;

public:
    User(string name, int id) : name(name), id(id), wallet(name) {}

    string getName()
    {
        return name;
    }

    void setName(string n)
    {
        name = n;
    }

    int getId()
    {
        return id;
    }

    void setId(int i)
    {
        id = i;
    }

    Wallet &getWallet()
    {
        return wallet;
    }

    void setSavingPlan(int id, double target, Date startDate, Date endDate) {
        plans.push_back(make_unique<SavingPlan>(id, target, startDate, endDate, name));
    }

    SavingPlan* getSavingPlan(int id) {
        for (auto& plan : plans) {
            if (plan->getId() == id) {
                return plan.get();
            }
        }
        Logger::getInstance()->log(Error, name, "Saving plan %d not found.", id);
        return nullptr;
    }

    void deleteSavingPlan(int Id) {
        auto it = find_if(plans.begin(), plans.end(), [Id](const unique_ptr<SavingPlan>& plan) {
            return plan->getId() == Id;
        });

        if (it != plans.end()) {
            wallet.addTransaction(Id, TransactionType::DEPOSIT , Categories::NONE, (*it)->getSavingAmount());
            Logger::getInstance()->log(Info, name, " Saving plan %d deleted successfully.", Id);
            plans.erase(it); 
        } else {
            Logger::getInstance()->log(Error, name, " Saving plan %d not found.", Id);
        }
    }

    void updateSavingPlan(int Id, double newTarget, Date newStartDate, Date newEndDate) {
        auto plan = getSavingPlan(Id);
        if (plan) {
            plan->setTarget(newTarget);
            plan->setStartDate(newStartDate);
            plan->setEndDate(newEndDate);
            Logger::getInstance()->log(Info, name, " Saving plan %d updated successfully.", Id);
        }else {
            Logger::getInstance()->log(Error, name, "Saving plan %d not found.", Id);
        }
    }
};