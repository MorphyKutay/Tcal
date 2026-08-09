#include "tasks.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

void tasklist_init(TaskList *list) {
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

void tasklist_free(TaskList *list) {
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

static int tasklist_ensure_capacity(TaskList *list, size_t needed) {
    if (needed <= list->capacity) {
        return 0;
    }
    size_t new_capacity = list->capacity == 0 ? 8 : list->capacity * 2;
    if (new_capacity < needed) {
        new_capacity = needed;
    }
    Task *items = realloc(list->items, new_capacity * sizeof(Task));
    if (items == NULL) {
        return -1;
    }
    list->items = items;
    list->capacity = new_capacity;
    return 0;
}

int tasklist_add(TaskList *list, const char *date, const char *text) {
    if (tasklist_ensure_capacity(list, list->count + 1) != 0) {
        return -1;
    }
    Task *t = &list->items[list->count];
    strncpy(t->date, date, TCAL_DATE_LEN);
    t->date[TCAL_DATE_LEN] = '\0';
    strncpy(t->text, text, TCAL_TEXT_MAX - 1);
    t->text[TCAL_TEXT_MAX - 1] = '\0';
    list->count++;
    return (int)(list->count - 1);
}

void tasklist_remove(TaskList *list, size_t index) {
    if (index >= list->count) {
        return;
    }
    for (size_t i = index; i + 1 < list->count; i++) {
        list->items[i] = list->items[i + 1];
    }
    list->count--;
}

size_t tasklist_count_for_date(const TaskList *list, const char *date) {
    size_t n = 0;
    for (size_t i = 0; i < list->count; i++) {
        if (strcmp(list->items[i].date, date) == 0) {
            n++;
        }
    }
    return n;
}

int tasklist_nth_for_date(const TaskList *list, const char *date, size_t n) {
    size_t seen = 0;
    for (size_t i = 0; i < list->count; i++) {
        if (strcmp(list->items[i].date, date) == 0) {
            if (seen == n) {
                return (int)i;
            }
            seen++;
        }
    }
    return -1;
}

int tasklist_load(TaskList *list, const char *path) {
    tasklist_free(list);
    tasklist_init(list);

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        return -1;
    }

    char line[TCAL_DATE_LEN + 1 + TCAL_TEXT_MAX + 2];
    while (fgets(line, sizeof(line), f) != NULL) {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[--len] = '\0';
        }
        if (len < TCAL_DATE_LEN + 1) {
            continue; /* too short to contain "YYYY-MM-DD|" */
        }
        if (line[TCAL_DATE_LEN] != '|') {
            continue; /* malformed: separator missing */
        }
        line[TCAL_DATE_LEN] = '\0';
        const char *date = line;
        const char *text = line + TCAL_DATE_LEN + 1;
        tasklist_add(list, date, text);
    }

    fclose(f);
    return 0;
}

int tasklist_save(const TaskList *list, const char *path) {
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        return -1;
    }
    for (size_t i = 0; i < list->count; i++) {
        fprintf(f, "%s|%s\n", list->items[i].date, list->items[i].text);
    }
    fclose(f);
    return 0;
}

int tcal_data_path(char *buf, size_t buflen) {
    const char *home = getenv("HOME");
    if (home == NULL) {
        return -1;
    }

    char dir[512];
    int n = snprintf(dir, sizeof(dir), "%s/.local/share/tcal", home);
    if (n < 0 || (size_t)n >= sizeof(dir)) {
        return -1;
    }

    char partial[512];
    n = snprintf(partial, sizeof(partial), "%s/.local", home);
    if (n < 0 || (size_t)n >= sizeof(partial)) return -1;
    if (mkdir(partial, 0755) != 0 && errno != EEXIST) return -1;

    n = snprintf(partial, sizeof(partial), "%s/.local/share", home);
    if (n < 0 || (size_t)n >= sizeof(partial)) return -1;
    if (mkdir(partial, 0755) != 0 && errno != EEXIST) return -1;

    if (mkdir(dir, 0755) != 0 && errno != EEXIST) {
        return -1;
    }

    n = snprintf(buf, buflen, "%s/tasks.txt", dir);
    if (n < 0 || (size_t)n >= buflen) {
        return -1;
    }
    return 0;
}
