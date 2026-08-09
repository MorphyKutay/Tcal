#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../src/tasks.h"

static int failures = 0;

#define CHECK(cond, msg) do { \
    if (!(cond)) { \
        printf("FAIL: %s\n", msg); \
        failures++; \
    } else { \
        printf("PASS: %s\n", msg); \
    } \
} while (0)

static void test_add_and_count(void) {
    TaskList list;
    tasklist_init(&list);

    tasklist_add(&list, "2026-08-09", "Diş randevusu");
    tasklist_add(&list, "2026-08-09", "Rapor teslim");
    tasklist_add(&list, "2026-08-10", "Toplantı");

    CHECK(list.count == 3, "three tasks added");
    CHECK(tasklist_count_for_date(&list, "2026-08-09") == 2, "two tasks on 08-09");
    CHECK(tasklist_count_for_date(&list, "2026-08-10") == 1, "one task on 08-10");
    CHECK(tasklist_count_for_date(&list, "2026-08-11") == 0, "no tasks on 08-11");

    int idx0 = tasklist_nth_for_date(&list, "2026-08-09", 0);
    int idx1 = tasklist_nth_for_date(&list, "2026-08-09", 1);
    int idx2 = tasklist_nth_for_date(&list, "2026-08-09", 2);
    CHECK(idx0 >= 0 && strcmp(list.items[idx0].text, "Diş randevusu") == 0, "nth 0 is first task");
    CHECK(idx1 >= 0 && strcmp(list.items[idx1].text, "Rapor teslim") == 0, "nth 1 is second task");
    CHECK(idx2 == -1, "nth 2 does not exist");

    tasklist_free(&list);
}

static void test_remove(void) {
    TaskList list;
    tasklist_init(&list);

    tasklist_add(&list, "2026-08-09", "A");
    tasklist_add(&list, "2026-08-09", "B");
    tasklist_add(&list, "2026-08-10", "C");

    int idx = tasklist_nth_for_date(&list, "2026-08-09", 0);
    tasklist_remove(&list, (size_t)idx);

    CHECK(list.count == 2, "one task removed, two remain");
    CHECK(tasklist_count_for_date(&list, "2026-08-09") == 1, "one task left on 08-09");
    CHECK(strcmp(list.items[0].text, "B") == 0, "remaining 08-09 task is B");
    CHECK(strcmp(list.items[1].text, "C") == 0, "08-10 task is still C");

    tasklist_free(&list);
}

static void test_save_and_load_roundtrip(void) {
    TaskList list;
    tasklist_init(&list);
    tasklist_add(&list, "2026-08-09", "Diş randevusu");
    tasklist_add(&list, "2026-08-10", "Rapor teslim");

    const char *path = "/tmp/tcal_test_tasks.txt";
    int save_rc = tasklist_save(&list, path);
    CHECK(save_rc == 0, "save succeeds");
    tasklist_free(&list);

    TaskList loaded;
    tasklist_init(&loaded);
    int load_rc = tasklist_load(&loaded, path);
    CHECK(load_rc == 0, "load succeeds");
    CHECK(loaded.count == 2, "two tasks loaded back");
    CHECK(strcmp(loaded.items[0].date, "2026-08-09") == 0, "first date roundtrips");
    CHECK(strcmp(loaded.items[0].text, "Diş randevusu") == 0, "first text roundtrips");
    CHECK(strcmp(loaded.items[1].date, "2026-08-10") == 0, "second date roundtrips");

    tasklist_free(&loaded);
    remove(path);
}

static void test_load_skips_malformed_lines(void) {
    const char *path = "/tmp/tcal_test_malformed.txt";
    FILE *f = fopen(path, "w");
    fprintf(f, "bad line without separator\n");
    fprintf(f, "2026-08-09|Good task\n");
    fprintf(f, "\n");
    fprintf(f, "2026-08-10|Another good task\n");
    fclose(f);

    TaskList list;
    tasklist_init(&list);
    int rc = tasklist_load(&list, path);

    CHECK(rc == 0, "load succeeds despite malformed lines");
    CHECK(list.count == 2, "malformed lines are skipped, two good ones kept");
    CHECK(strcmp(list.items[0].text, "Good task") == 0, "first good task parsed");
    CHECK(strcmp(list.items[1].text, "Another good task") == 0, "second good task parsed");

    tasklist_free(&list);
    remove(path);
}

static void test_load_missing_file(void) {
    TaskList list;
    tasklist_init(&list);
    int rc = tasklist_load(&list, "/tmp/tcal_test_does_not_exist.txt");
    CHECK(rc == -1, "load reports failure for missing file");
    CHECK(list.count == 0, "list stays empty when file is missing");
    tasklist_free(&list);
}

int main(void) {
    test_add_and_count();
    test_remove();
    test_save_and_load_roundtrip();
    test_load_skips_malformed_lines();
    test_load_missing_file();

    if (failures > 0) {
        printf("\n%d check(s) failed.\n", failures);
        return 1;
    }
    printf("\nAll checks passed.\n");
    return 0;
}
