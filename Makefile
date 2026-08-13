ifeq ($(OS),Windows_NT)
    CC      = gcc
    CFLAGS  = -Wall -Wextra -std=c11 -g
    LIBS    = -lpdcurses
    EXE_EXT = .exe
    RM      = del /Q /F
    MKDIR   = if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
    RMDIR   = if exist $(OBJ_DIR) rmdir /S /Q $(OBJ_DIR)
else
    CC      = gcc
    CFLAGS  = -Wall -Wextra -std=c11 -g
    LIBS    = -lncurses
    EXE_EXT =
    RM      = rm -f
    MKDIR   = mkdir -p $(OBJ_DIR)
    RMDIR   = rm -rf $(OBJ_DIR)
endif

SRC = src/calendar.c src/util.c src/tasks.c src/ui.c src/main.c
OBJ_DIR = build
OBJ = $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRC))

.PHONY: all test clean

all: tcal

$(OBJ_DIR):
	$(MKDIR)

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

tcal: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o tcal$(EXE_EXT) $(LIBS)



clean:
	$(RMDIR)
	 $(RM) tcal$(EXE_EXT) test_calendar$(EXE_EXT) test_util$(EXE_EXT) test_tasks$(EXE_EXT)