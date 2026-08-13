#ifndef TCAL_UI_H
#define TCAL_UI_H

#include "calendar.h"
#include "tasks.h"

typedef enum {
    TCAL_MODE_NORMAL,
    TCAL_MODE_COMMAND,
    TCAL_MODE_INPUT,
    TCAL_MODE_VIEW,
    TCAL_MODE_DONE,
} AppMode;

typedef struct {
    int year;
    int month;    /* 1-12 */
    int sel_day;  /* 1..days_in_month(year, month) */
    int sel_task; /* -1 if the day itself is selected, else 0-based index of
                     the selected task within sel_day's tasks (in list order) */
    TcalLang lang;
} ViewState;

void ui_init(void);
void ui_teardown(void);

/* Draws the full screen: month grid, and the bottom status/command line.
   `status` is shown on the last line depending on mode:
     - TCAL_MODE_NORMAL: shown as-is (a short hint string)
     - TCAL_MODE_COMMAND: shown as ":" followed by `status` (the command
       buffer typed so far)
     - TCAL_MODE_INPUT: shown as "Görev: " followed by `status` (the task
       text typed so far) */
void ui_draw(const ViewState *view, const TaskList *tasks, AppMode mode, const char *status);

#endif
