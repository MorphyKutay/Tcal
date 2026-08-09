#ifndef TCAL_CALENDAR_H
#define TCAL_CALENDAR_H

int tcal_is_leap_year(int year);
int tcal_days_in_month(int year, int month);
/* 0 = Monday ... 6 = Sunday, weekday of the 1st of (year, month) */
int tcal_first_weekday_of_month(int year, int month);
const char *tcal_month_name(int month);       /* 1-12, Turkish */
const char *tcal_weekday_short(int index);    /* 0=Pt..6=Pz */
void tcal_add_months(int *year, int *month, int delta);

#endif
