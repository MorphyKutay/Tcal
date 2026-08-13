# TCal

A terminal calendar with vim-style modal keybindings, built with C and ncurses.
Navigate a month grid with `hjkl`, drop into command mode with `:`, and manage
tasks per day without leaving the keyboard.

![tcal demo (English)](demo-en.gif)

## Features

- Monthly grid view, week starts on Monday, shared-border table layout
- vim-style navigation: `hjkl` / arrow keys move between days and, within a
  day, between its tasks
- Command mode (`:`) for adding (`:e`), viewing full text (`:v`), toggling
  done (`:l`), deleting (`:d`), and quitting (`:q`)
- Completed tasks are marked with a `+` in the calendar grid
- Tasks persist to `~/.local/share/tcal/tasks.txt`
- Turkish by default, English via `TCAL_LANG=en`

## Build

Requires `gcc` and `ncurses`.

```sh
make
./tcal
```

Run the automated tests (pure logic modules: date math, UTF-8 helpers, task
storage):

```sh
make test
```

## Usage

| Key         | Action                                              |
|-------------|------------------------------------------------------|
| `h` `j` `k` `l` / arrows | Move between days; `j`/`k` also step into a day's tasks |
| `p` / `n`   | Previous / next month                                |
| `:`         | Enter command mode                                   |
| `:e`        | Add a task to the selected day (opens a text prompt) |
| `:v`        | View the selected task's full text (untruncated)     |
| `:l`        | Toggle the selected task's done status (shown as `+`) |
| `:d`        | Delete the selected task                             |
| `:q`        | Save and quit                                        |
| `Esc`       | Cancel command / input mode                          |

Run in English:

```sh
TCAL_LANG=en ./tcal
```

## Project layout

```
src/
  calendar.c/.h   date math + month/weekday names (pure, tested)
  util.c/.h       UTF-8 display-width helpers (pure, tested)
  tasks.c/.h      task storage, load/save (pure, tested)
  ui.c/.h         ncurses grid rendering
  main.c          event loop, modes, keybindings
tests/            automated tests for the pure modules above
docs/superpowers/ design doc
```

## Data

Tasks are stored as plain text at `~/.local/share/tcal/tasks.txt`, one per
line: `YYYY-MM-DD|0 or 1|task text` (the middle field is the done status).

## License

This project is licensed under the GNU General Public License v3.0.
See the [LICENSE](LICENSE) file for details.