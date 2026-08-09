#include "calendar.h"

int tcal_is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int tcal_days_in_month(int year, int month) {
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && tcal_is_leap_year(year)) {
        return 29;
    }
    return days[month - 1];
}

int tcal_first_weekday_of_month(int year, int month) {
    static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    int y = year;
    if (month < 3) {
        y -= 1;
    }
    int sunday_index = (y + y / 4 - y / 100 + y / 400 + t[month - 1] + 1) % 7;
    /* sunday_index: 0=Sunday..6=Saturday. Convert to Monday-indexed. */
    return (sunday_index + 6) % 7;
}

const char *tcal_month_name(int month, TcalLang lang) {
    static const char *names_tr[] = {
        "Ocak", "Şubat", "Mart", "Nisan", "Mayıs", "Haziran",
        "Temmuz", "Ağustos", "Eylül", "Ekim", "Kasım", "Aralık"
    };
    static const char *names_en[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    return lang == TCAL_LANG_EN ? names_en[month - 1] : names_tr[month - 1];
}

const char *tcal_weekday_short(int index, TcalLang lang) {
    static const char *names_tr[] = {"Pt", "Sa", "Ça", "Pe", "Cu", "Ct", "Pz"};
    static const char *names_en[] = {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"};
    return lang == TCAL_LANG_EN ? names_en[index] : names_tr[index];
}

void tcal_add_months(int *year, int *month, int delta) {
    int total = (*year * 12 + (*month - 1)) + delta;
    *year = total / 12;
    *month = total % 12;
    if (*month < 0) {
        *month += 12;
        *year -= 1;
    }
    *month += 1;
}
