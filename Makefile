CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LIBS = -lncurses

SRC = src/calendar.c src/util.c src/tasks.c src/ui.c src/main.c
OBJ_DIR = build
OBJ = $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRC))

.PHONY: all test clean

all: tcal

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

tcal: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LIBS)

test: test_calendar test_util test_tasks
	./test_calendar
	./test_util
	./test_tasks

test_calendar: tests/test_calendar.c src/calendar.c
	$(CC) $(CFLAGS) $^ -o $@

test_util: tests/test_util.c src/util.c
	$(CC) $(CFLAGS) $^ -o $@

test_tasks: tests/test_tasks.c src/tasks.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(OBJ_DIR) tcal test_calendar test_util test_tasks
