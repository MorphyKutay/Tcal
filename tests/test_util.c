#include <stdio.h>
#include <string.h>
#include "../src/util.h"

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
    CHECK(utf8_display_width("Pt") == 2, "ASCII width 2");
    CHECK(utf8_display_width("Ça") == 2, "Ç counts as one column");
    CHECK(utf8_display_width("Ağustos") == 7, "Ağustos is 7 columns");
    CHECK(utf8_display_width("") == 0, "empty string width 0");

    char buf[32];

    utf8_truncate(buf, sizeof(buf), "Rapor teslim", 5);
    CHECK(strcmp(buf, "Rapor") == 0, "truncate ASCII to 5 columns");

    utf8_truncate(buf, sizeof(buf), "Ça günü", 2);
    CHECK(strcmp(buf, "Ça") == 0, "truncate keeps multibyte char whole");

    utf8_truncate(buf, sizeof(buf), "Diş randevusu", 3);
    CHECK(strcmp(buf, "Diş") == 0, "truncate keeps first 3 codepoints including multibyte ş");

    utf8_truncate(buf, sizeof(buf), "kısa", 100);
    CHECK(strcmp(buf, "kısa") == 0, "truncate is no-op when width exceeds string");

    if (failures > 0) {
        printf("\n%d check(s) failed.\n", failures);
        return 1;
    }
    printf("\nAll checks passed.\n");
    return 0;
}
