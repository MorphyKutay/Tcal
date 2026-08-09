#include <stdio.h>
#include <string.h>
#include "../src/calendar.h"

static int failures = 0;

#define CHECK(cond, msg) do { \
    if (!(cond)) { \
        printf("FAIL: %s\n", msg); \
        failures++; \
    } else { \
        printf("PASS: %s\n", msg); \
    } \
} while (0)

int main(void) {
    CHECK(tcal_is_leap_year(2000) == 1, "2000 is leap year");
    CHECK(tcal_is_leap_year(1900) == 0, "1900 is not leap year");
    CHECK(tcal_is_leap_year(2024) == 1, "2024 is leap year");
    CHECK(tcal_is_leap_year(2023) == 0, "2023 is not leap year");

    CHECK(tcal_days_in_month(2026, 8) == 31, "August has 31 days");
    CHECK(tcal_days_in_month(2026, 4) == 30, "April has 30 days");
    CHECK(tcal_days_in_month(2024, 2) == 29, "Feb 2024 (leap) has 29 days");
    CHECK(tcal_days_in_month(2023, 2) == 28, "Feb 2023 (non-leap) has 28 days");

    /* 2026-08-01 is a Saturday -> Monday-indexed 5 */
    CHECK(tcal_first_weekday_of_month(2026, 8) == 5, "Aug 1 2026 is Saturday (index 5)");
    /* 2026-01-01 is a Thursday -> Monday-indexed 3 */
    CHECK(tcal_first_weekday_of_month(2026, 1) == 3, "Jan 1 2026 is Thursday (index 3)");
    /* 2000-01-01 is a Saturday -> Monday-indexed 5 */
    CHECK(tcal_first_weekday_of_month(2000, 1) == 5, "Jan 1 2000 is Saturday (index 5)");

    CHECK(strcmp(tcal_month_name(8, TCAL_LANG_TR), "Ağustos") == 0, "month 8 name (tr) is Ağustos");
    CHECK(strcmp(tcal_month_name(1, TCAL_LANG_TR), "Ocak") == 0, "month 1 name (tr) is Ocak");
    CHECK(strcmp(tcal_weekday_short(0, TCAL_LANG_TR), "Pt") == 0, "weekday 0 short (tr) is Pt");
    CHECK(strcmp(tcal_weekday_short(6, TCAL_LANG_TR), "Pz") == 0, "weekday 6 short (tr) is Pz");

    CHECK(strcmp(tcal_month_name(8, TCAL_LANG_EN), "August") == 0, "month 8 name (en) is August");
    CHECK(strcmp(tcal_month_name(1, TCAL_LANG_EN), "January") == 0, "month 1 name (en) is January");
    CHECK(strcmp(tcal_weekday_short(0, TCAL_LANG_EN), "Mo") == 0, "weekday 0 short (en) is Mo");
    CHECK(strcmp(tcal_weekday_short(6, TCAL_LANG_EN), "Su") == 0, "weekday 6 short (en) is Su");

    {
        int y = 2026, m = 1;
        tcal_add_months(&y, &m, -1);
        CHECK(y == 2025 && m == 12, "Jan 2026 - 1 month = Dec 2025");
    }
    {
        int y = 2026, m = 12;
        tcal_add_months(&y, &m, 1);
        CHECK(y == 2027 && m == 1, "Dec 2026 + 1 month = Jan 2027");
    }
    {
        int y = 2026, m = 8;
        tcal_add_months(&y, &m, 0);
        CHECK(y == 2026 && m == 8, "adding 0 months is a no-op");
    }

    if (failures > 0) {
        printf("\n%d check(s) failed.\n", failures);
        return 1;
    }
    printf("\nAll checks passed.\n");
    return 0;
}
