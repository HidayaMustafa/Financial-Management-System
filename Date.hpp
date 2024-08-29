#pragma once

#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

using namespace std;

class Date {
private:
    int day;
    int month;
    int year;

public:
    Date();
    Date(int d, int m, int y);

    bool operator>=(const Date& d) const;
    bool operator<=(const Date& d) const;
    int operator-(const Date& d) const;

    void display() const;
};
