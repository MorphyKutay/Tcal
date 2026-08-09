#ifndef TCAL_CALENDAR_H
#define TCAL_CALENDAR_H

typedef enum {
    TCAL_LANG_TR,
    TCAL_LANG_EN
} TcalLang;

int tcal_is_leap_year(int year);
int tcal_days_in_month(int year, int month);
/* 0 = Monday ... 6 = Sunday, weekday of the 1st of (year, month) */
int tcal_first_weekday_of_month(int year, int month);
const char *tcal_month_name(int month, TcalLang lang);    /* 1-12 */
const char *tcal_weekday_short(int index, TcalLang lang); /* 0=Mon..6=Sun */
void tcal_add_months(int *year, int *month, int delta);

#endif
