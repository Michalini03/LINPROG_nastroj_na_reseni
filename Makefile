CC = gcc
CFLAGS = -Wall -Wextra -pedantic -ansi -g
LDFLAGS = $(CFLAGS) -lm

BUILD_DIR = build
BIN = lp.exe
ARG = "test.txt"

all: clean $(BUILD_DIR) $(BUILD_DIR)/$(BIN)

$(BUILD_DIR)/$(BIN): $(BUILD_DIR)/lp.o $(BUILD_DIR)/lpfile.o
	$(CC) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/lp.o: src/main.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/lpfile.o: src/lpfile.c src/lpfile.h
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR):
	mkdir $@

# na Windows použijte del /F /Q $(BUILD_DIR)
clean:
	rm -rf $(BUILD_DIR)

run: all
	./$(BUILD_DIR)/$(BIN) $(ARG)