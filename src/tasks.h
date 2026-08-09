#ifndef TCAL_TASKS_H
#define TCAL_TASKS_H

#include <stddef.h>

#define TCAL_DATE_LEN 10   /* "YYYY-MM-DD" without NUL */
#define TCAL_TEXT_MAX 256

typedef struct {
    char date[TCAL_DATE_LEN + 1];
    char text[TCAL_TEXT_MAX];
} Task;

typedef struct {
    Task *items;
    size_t count;
    size_t capacity;
} TaskList;

void tasklist_init(TaskList *list);
void tasklist_free(TaskList *list);

/* Appends a task, truncating text if longer than TCAL_TEXT_MAX-1. Returns the
   new task's index, or -1 on allocation failure. */
int tasklist_add(TaskList *list, const char *date, const char *text);

/* Removes the task at global index `index`. No-op if index is out of range. */
void tasklist_remove(TaskList *list, size_t index);

/* Number of tasks whose date equals `date`. */
size_t tasklist_count_for_date(const TaskList *list, const char *date);

/* Returns the global index of the n-th (0-based, in list order) task for
   `date`, or -1 if there are fewer than n+1 tasks on that date. */
int tasklist_nth_for_date(const TaskList *list, const char *date, size_t n);

/* Loads tasks from `path` into `list` (which must already be initialized;
   existing contents are cleared first). Malformed lines are skipped.
   Returns 0 on success, -1 if the file could not be read (list is left
   empty; this is not fatal, the file may simply not exist yet). */
int tasklist_load(TaskList *list, const char *path);

/* Writes all tasks in `list` to `path`, one per line as
   "YYYY-MM-DD|text\n". Returns 0 on success, -1 on write failure. */
int tasklist_save(const TaskList *list, const char *path);

/* Fills `buf` (size `buflen`) with the absolute path to the tasks data
   file (~/.local/share/tcal/tasks.txt) and ensures its parent directory
   exists. Returns 0 on success, -1 on failure. */
int tcal_data_path(char *buf, size_t buflen);

#endif
