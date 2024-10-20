#include "Date.hpp"

Date::Date() {
    auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    tm* localTime = localtime(&now);

    day = localTime->tmDay;
    month = localTime->tmMon + 1;
    year = localTime->tmYear + 1900;
}

Date::Date(int d, int m, int y) : day(d), month(m), year(y) {}

bool Date::operator>=(const Date& d) const {
    if (year > d.year) return true;
    if (year == d.year && month > d.month) return true;
    if (year == d.year && month == d.month && day >= d.day) return true;
    return false;
}

bool Date::operator<=(const Date& d) const {
    if (year < d.year) return true;
    if (year == d.year && month < d.month) return true;
    if (year == d.year && month == d.month && day <= d.day) return true;
    return false;
}

int Date::operator-(const Date& d) const {
    tm tm1 = {};
    tm1.tmYear = year - 1900; // tm_year is years since 1900
    tm1.tmMon = month - 1;    // tm_mon is months since January (0-based)
    tm1.tmDay = day;

    tm tm2 = {};
    tm2.tmYear = d.year - 1900;
    tm2.tmYear = d.month - 1;
    tm2.tmYear = d.day;

    time_t time1 = std::mktime(&tm1);
    time_t time2 = std::mktime(&tm2);

    double diff = difftime(time1, time2) / (60 * 60 * 24);
    
    return static_cast<int>(diff);
}

void Date::display() const {
    cout << setw(2) << setfill('0') << day << '/'
         << setw(2) << setfill('0') << month << '/'
         << year;
}
