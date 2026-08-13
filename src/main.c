#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "calendar.h"
#include "tasks.h"
#include "ui.h"

#define CMD_BUF_SIZE 64
#define INPUT_BUF_SIZE TCAL_TEXT_MAX

static void current_date_str(const ViewState *view, char *out, size_t outlen) {
    snprintf(out, outlen, "%04d-%02d-%02d", view->year, view->month, view->sel_day);
}

static void clamp_sel_day(ViewState *view) {
    int max_day = tcal_days_in_month(view->year, view->month);
    if (view->sel_day > max_day) {
        view->sel_day = max_day;
    }
    if (view->sel_day < 1) {
        view->sel_day = 1;
    }
}

static void move_day(ViewState *view, int delta) {
    view->sel_task = -1;
    int max_day = tcal_days_in_month(view->year, view->month);
    int new_day = view->sel_day + delta;
    if (new_day < 1) new_day = 1;
    if (new_day > max_day) new_day = max_day;
    view->sel_day = new_day;
}

static void move_month(ViewState *view, int delta) {
    tcal_add_months(&view->year, &view->month, delta);
    view->sel_task = -1;
    clamp_sel_day(view);
}

static void move_down(ViewState *view, const TaskList *tasks) {
    char date[TCAL_DATE_LEN + 1];
    current_date_str(view, date, sizeof(date));
    size_t total = tasklist_count_for_date(tasks, date);
    int max_day = tcal_days_in_month(view->year, view->month);

    if (view->sel_task < 0) {
        if (total > 0) {
            view->sel_task = 0;
        } else if (view->sel_day + 7 <= max_day) {
            view->sel_day += 7;
        }
    } else {
        if ((size_t)(view->sel_task + 1) < total) {
            view->sel_task += 1;
        } else {
            view->sel_task = -1;
            if (view->sel_day + 7 <= max_day) {
                view->sel_day += 7;
            }
        }
    }
}

static void move_up(ViewState *view) {
    if (view->sel_task < 0) {
        if (view->sel_day - 7 >= 1) {
            view->sel_day -= 7;
        }
    } else if (view->sel_task > 0) {
        view->sel_task -= 1;
    } else {
        view->sel_task = -1;
    }
}

static void handle_command(char *cmd_buf, ViewState *view, TaskList *tasks,
                            const char *data_path, AppMode *mode, char *input_buf, char *view_buf,char *done_buf,
                            int *should_quit) {
    if (strcmp(cmd_buf, "e") == 0) {
        *mode = TCAL_MODE_INPUT;
        input_buf[0] = '\0';
        return;
    }

    if (strcmp(cmd_buf, "v") == 0) {

        if (view->sel_task >= 0) {
            char date[TCAL_DATE_LEN + 1];
            current_date_str(view, date, sizeof(date));
            int idx = tasklist_nth_for_date(tasks, date, (size_t)view->sel_task);

            if (idx >= 0) {
                strncpy(view_buf, tasks->items[idx].text, INPUT_BUF_SIZE - 1);
                view_buf[INPUT_BUF_SIZE -1 ] = '\0';
                *mode = TCAL_MODE_VIEW;
                return;
            }

        }

        *mode = TCAL_MODE_NORMAL;
        return;
    }

    if (strcmp(cmd_buf,"l") == 0) {
        if (view->sel_task >= 0) {
            char date[TCAL_DATE_LEN + 1];
            current_date_str(view, date, sizeof(date));
            int idx = tasklist_nth_for_date(tasks, date, (size_t)view->sel_task);
            if (idx >= 0) {
                tasks->items[idx].done = !tasks->items[idx].done;
                tasklist_save(tasks,data_path);
            }
        }
        *mode = TCAL_MODE_NORMAL;
        return;
    }

    if (strcmp(cmd_buf, "d") == 0) {
        if (view->sel_task >= 0) {
            char date[TCAL_DATE_LEN + 1];
            current_date_str(view, date, sizeof(date));
            int idx = tasklist_nth_for_date(tasks, date, (size_t)view->sel_task);
            if (idx >= 0) {
                tasklist_remove(tasks, (size_t)idx);
                tasklist_save(tasks, data_path);
                size_t new_count = tasklist_count_for_date(tasks, date);
                if ((size_t)view->sel_task >= new_count) {
                    view->sel_task = new_count > 0 ? (int)new_count - 1 : -1;
                }
            }
        }
        *mode = TCAL_MODE_NORMAL;
        return;
    }

    if (strcmp(cmd_buf, "q") == 0) {
        tasklist_save(tasks, data_path);
        *should_quit = 1;
        return;
    }

    *mode = TCAL_MODE_NORMAL;
}

int main(void) {
    const char *lang_env = getenv("TCAL_LANG");
    TcalLang lang = (lang_env != NULL && strcmp(lang_env, "en") == 0) ? TCAL_LANG_EN : TCAL_LANG_TR;

    char data_path[512];
    if (tcal_data_path(data_path, sizeof(data_path)) != 0) {
        fprintf(stderr, lang == TCAL_LANG_EN
                             ? "tcal: could not determine data file path (is HOME set?)\n"
                             : "tcal: veri dosyasi yolu belirlenemedi (HOME ayarli mi?)\n");
        return 1;
    }

    TaskList tasks;
    tasklist_init(&tasks);
    tasklist_load(&tasks, data_path); /* dosya yoksa liste bos baslar, sorun degil */

    time_t now = time(NULL);
    struct tm *lt = localtime(&now);

    ViewState view;
    view.year = lt->tm_year + 1900;
    view.month = lt->tm_mon + 1;
    view.sel_day = lt->tm_mday;
    view.sel_task = -1;
    view.lang = lang;

    ui_init();

    AppMode mode = TCAL_MODE_NORMAL;
    char cmd_buf[CMD_BUF_SIZE] = "";
    char input_buf[INPUT_BUF_SIZE] = "";
    char view_buf[INPUT_BUF_SIZE] = "";
    char done_buf[INPUT_BUF_SIZE] = "";
    size_t cmd_len = 0;
    size_t input_len = 0;
    int should_quit = 0;

    const char *normal_hint = lang == TCAL_LANG_EN ? "hjkl: move  p/n: month  ':' command"
                                                     : "hjkl: gez  p/n: ay  ':' komut";

    while (!should_quit) {
        const char *status;
        switch (mode) {
            case TCAL_MODE_NORMAL: status = normal_hint; break;
            case TCAL_MODE_COMMAND: status = cmd_buf; break;
            case TCAL_MODE_INPUT: status = input_buf; break;
            case TCAL_MODE_VIEW: status = view_buf; break;
            case TCAL_MODE_DONE: status = done_buf; break;
            default: status = ""; break;
        }
        ui_draw(&view, &tasks, mode, status);

        int ch = getch();

        if (mode == TCAL_MODE_NORMAL) {
            switch (ch) {
                case 'h': case KEY_LEFT: move_day(&view, -1); break;
                case 'l': case KEY_RIGHT: move_day(&view, 1); break;
                case 'j': case KEY_DOWN: move_down(&view, &tasks); break;
                case 'k': case KEY_UP: move_up(&view); break;
                case 'p': move_month(&view, -1); break;
                case 'n': move_month(&view, 1); break;
                case ':':
                    mode = TCAL_MODE_COMMAND;
                    cmd_buf[0] = '\0';
                    cmd_len = 0;
                    break;
                default: break;
            }
        } else if (mode == TCAL_MODE_COMMAND) {
            if (ch == 27) {
                mode = TCAL_MODE_NORMAL;
                cmd_buf[0] = '\0';
                cmd_len = 0;
            } else if (ch == '\n' || ch == '\r' || ch == KEY_ENTER) {
                handle_command(cmd_buf, &view, &tasks, data_path, &mode, input_buf, view_buf,done_buf,&should_quit);
                cmd_buf[0] = '\0';
                cmd_len = 0;
            } else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
                if (cmd_len > 0) {
                    cmd_len--;
                    cmd_buf[cmd_len] = '\0';
                }
            } else if (ch >= 32 && ch <= 255 && cmd_len + 1 < sizeof(cmd_buf)) {
                cmd_buf[cmd_len++] = (char)ch;
                cmd_buf[cmd_len] = '\0';
            }
        } else if (mode == TCAL_MODE_INPUT) {
            if (ch == 27) {
                mode = TCAL_MODE_NORMAL;
                input_buf[0] = '\0';
                input_len = 0;
            } else if (ch == '\n' || ch == '\r' || ch == KEY_ENTER) {
                if (input_len > 0) {
                    char date[TCAL_DATE_LEN + 1];
                    current_date_str(&view, date, sizeof(date));
                    tasklist_add(&tasks, date, input_buf);
                    tasklist_save(&tasks, data_path);
                }
                mode = TCAL_MODE_NORMAL;
                input_buf[0] = '\0';
                input_len = 0;
            } else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
                if (input_len > 0) {
                    input_len--;
                    input_buf[input_len] = '\0';
                }
            } else if (ch >= 32 && ch <= 255 && input_len + 1 < sizeof(input_buf)) {
                input_buf[input_len++] = (char)ch;
                input_buf[input_len] = '\0';
            }
        }

        else if (mode == TCAL_MODE_VIEW) {
            mode = TCAL_MODE_NORMAL;
            view_buf[0] = '\0';
        }
    }

    ui_teardown();
    tasklist_free(&tasks);
    return 0;
}
