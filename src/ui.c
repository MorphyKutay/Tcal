#include "ui.h"
#include "calendar.h"
#include "util.h"

#include <locale.h>
#include <ncurses.h>
#include <string.h>

#define CELL_WIDTH 8
#define GRID_COLS 7
#define MAX_WEEKS 6

static int build_weeks(int year, int month, int weeks[MAX_WEEKS][GRID_COLS]) {
    int first_weekday = tcal_first_weekday_of_month(year, month); /* 0=Mon..6=Sun */
    int days = tcal_days_in_month(year, month);

    for (int w = 0; w < MAX_WEEKS; w++) {
        for (int c = 0; c < GRID_COLS; c++) {
            weeks[w][c] = 0;
        }
    }

    int day = 1;
    int week_count = 0;
    for (int w = 0; w < MAX_WEEKS && day <= days; w++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (w == 0 && c < first_weekday) {
                weeks[w][c] = 0;
            } else if (day <= days) {
                weeks[w][c] = day;
                day++;
            } else {
                weeks[w][c] = 0;
            }
        }
        week_count++;
    }
    return week_count;
}

static int grid_total_width(void) {
    return 1 + GRID_COLS * (CELL_WIDTH + 1);
}

static void draw_title_row(int row, const char *title) {
    int width = grid_total_width();
    int inner = width - 2;
    int title_len = (int)utf8_display_width(title);

    int pad_total = inner - title_len - 2;
    if (pad_total < 0) {
        pad_total = 0;
    }
    int left_pad = pad_total / 2;
    int right_pad = pad_total - left_pad;

    move(row, 0);
    addch(ACS_ULCORNER);
    for (int i = 0; i < left_pad; i++) addch(ACS_HLINE);
    addch(' ');
    addstr(title);
    addch(' ');
    for (int i = 0; i < right_pad; i++) addch(ACS_HLINE);
    addch(ACS_URCORNER);
}

static void draw_weekday_header(int row, TcalLang lang) {
    move(row, 0);
    addch(ACS_VLINE);
    for (int c = 0; c < GRID_COLS; c++) {
        const char *label = tcal_weekday_short(c, lang);
        int label_width = (int)utf8_display_width(label);
        int pad_total = CELL_WIDTH - label_width;
        int left_pad = pad_total / 2;
        int right_pad = pad_total - left_pad;
        for (int i = 0; i < left_pad; i++) addch(' ');
        addstr(label);
        for (int i = 0; i < right_pad; i++) addch(' ');
        addch(ACS_VLINE);
    }
}

static void draw_border_row(int row, chtype left, chtype mid, chtype right) {
    move(row, 0);
    addch(left);
    for (int c = 0; c < GRID_COLS; c++) {
        for (int i = 0; i < CELL_WIDTH; i++) {
            addch(ACS_HLINE);
        }
        addch(c == GRID_COLS - 1 ? right : mid);
    }
}

static void render_day_cell_content(char lines[2][CELL_WIDTH + 1], int day, int year, int month,
                                     const TaskList *tasks, TcalLang lang) {
    memset(lines[0], ' ', CELL_WIDTH);
    lines[0][CELL_WIDTH] = '\0';
    memset(lines[1], ' ', CELL_WIDTH);
    lines[1][CELL_WIDTH] = '\0';

    if (day == 0) {
        return;
    }

    char date[TCAL_DATE_LEN + 1];
    snprintf(date, sizeof(date), "%04d-%02d-%02d", year, month, day);
    size_t total = tasklist_count_for_date(tasks, date);

    if (total >= 1) {
        int idx0 = tasklist_nth_for_date(tasks, date, 0);
        char label[TCAL_TEXT_MAX + 4];
        const char *marker0 = tasks->items[idx0].done ? "+" : "*";
        snprintf(label, sizeof(label), "%s%s", marker0, tasks->items[idx0].text);
        char trunc[CELL_WIDTH + 1];
        utf8_truncate(trunc, sizeof(trunc), label, CELL_WIDTH);
        memcpy(lines[0], trunc, strlen(trunc));
    }

    if (total == 2) {
        int idx1 = tasklist_nth_for_date(tasks, date, 1);
        char label[TCAL_TEXT_MAX + 4];
        const char *marker1 = tasks->items[idx1].done ? "+" : "*";
        snprintf(label, sizeof(label), "%s%s", marker1, tasks->items[idx1].text);
        char trunc[CELL_WIDTH + 1];
        utf8_truncate(trunc, sizeof(trunc), label, CELL_WIDTH);
        memcpy(lines[1], trunc, strlen(trunc));
    } else if (total > 2) {
        char label[32];
        snprintf(label, sizeof(label), lang == TCAL_LANG_EN ? "+%zu more" : "+%zu daha", total - 1);
        char trunc[CELL_WIDTH + 1];
        utf8_truncate(trunc, sizeof(trunc), label, CELL_WIDTH);
        memcpy(lines[1], trunc, strlen(trunc));
    }
}

void ui_init(void) {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
}

void ui_teardown(void) {
    endwin();
}

void ui_draw(const ViewState *view, const TaskList *tasks, AppMode mode, const char *status) {
    erase();

    int weeks[MAX_WEEKS][GRID_COLS];
    int week_count = build_weeks(view->year, view->month, weeks);

    int needed_width = grid_total_width();
    int needed_height = 3 + week_count * 4 + 2;

    if (LINES < needed_height || COLS < needed_width) {
        move(0, 0);
        addstr(view->lang == TCAL_LANG_EN ? "Terminal too small, please resize."
                                           : "Terminal cok kucuk, lutfen buyutun.");
        refresh();
        return;
    }

    char title[64];
    snprintf(title, sizeof(title), "%s %d", tcal_month_name(view->month, view->lang), view->year);

    int row = 0;
    draw_title_row(row, title);
    row++;

    draw_weekday_header(row, view->lang);
    row++;

    draw_border_row(row, ACS_LTEE, ACS_TTEE, ACS_RTEE);
    row++;

    for (int w = 0; w < week_count; w++) {
        move(row, 0);
        addch(ACS_VLINE);
        for (int c = 0; c < GRID_COLS; c++) {
            int day = weeks[w][c];
            int is_selected = (day != 0 && day == view->sel_day);
            char num[CELL_WIDTH + 1];
            memset(num, ' ', CELL_WIDTH);
            num[CELL_WIDTH] = '\0';
            if (day != 0) {
                char digits[8];
                snprintf(digits, sizeof(digits), "%d", day);
                int len = (int)strlen(digits);
                memcpy(num + (CELL_WIDTH - len), digits, len);
            }
            if (is_selected) attron(A_REVERSE);
            addstr(num);
            if (is_selected) attroff(A_REVERSE);
            addch(ACS_VLINE);
        }
        row++;

        char cell_lines[GRID_COLS][2][CELL_WIDTH + 1];
        for (int c = 0; c < GRID_COLS; c++) {
            render_day_cell_content(cell_lines[c], weeks[w][c], view->year, view->month, tasks, view->lang);
        }

        for (int t = 0; t < 2; t++) {
            move(row, 0);
            addch(ACS_VLINE);
            for (int c = 0; c < GRID_COLS; c++) {
                int day = weeks[w][c];
                int is_selected_day = (day != 0 && day == view->sel_day);
                int is_selected_task_line = is_selected_day && view->sel_task >= 0 &&
                                             ((t == 0 && view->sel_task == 0) ||
                                              (t == 1 && view->sel_task >= 1));

                if (is_selected_day) attron(A_REVERSE);
                if (is_selected_task_line) attron(A_BOLD);
                addstr(cell_lines[c][t]);
                if (is_selected_task_line) attroff(A_BOLD);
                if (is_selected_day) attroff(A_REVERSE);
                addch(ACS_VLINE);
            }
            row++;
        }

        draw_border_row(row,
                         w == week_count - 1 ? ACS_LLCORNER : ACS_LTEE,
                         w == week_count - 1 ? ACS_BTEE : ACS_PLUS,
                         w == week_count - 1 ? ACS_LRCORNER : ACS_RTEE);
        row++;
    }

    row++;
    move(row, 0);
    clrtoeol();
    switch (mode) {
        case TCAL_MODE_NORMAL:
            addstr(status);
            break;
        case TCAL_MODE_COMMAND:
            addch(':');
            addstr(status);
            break;
        case TCAL_MODE_INPUT:
            addstr(view->lang == TCAL_LANG_EN ? "Task: " : "Görev: ");
            addstr(status);
            break;
        case TCAL_MODE_VIEW:
            addstr(status);
            break;
        case TCAL_MODE_DONE:
            addstr(status);
            break;
    }

    refresh();
}
