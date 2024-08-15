#include <iostream>
#include <string>
#include "Wallet.cpp"

using namespace std;

class User {
private:
    string name;
    int id;
    Wallet wallet;

public:
    User(string name, int id) : name(name), id(id) {}

    string getName() const {
        return name;
    }

    void setName(string n) {
        name = n;
    }

    int getId() const {
        return id;
    }

    void setId(int i) {
        id = i;
    }

    Wallet &getWallet() {
        return wallet;
    }
};

