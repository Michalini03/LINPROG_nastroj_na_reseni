CC = gcc
CFLAGS = -Wall -Wextra -pedantic -ansi -g
LDFLAGS = $(CFLAGS) -lm

BIN = lp.exe
ARG = "./input.txt"
ARG2 = "-o"
ARG3 = "./output.txt"

all: clean $(BUILD_DIR) $(BIN)

$(BIN): lp.o lpfile.o simplex.o shunt.o
	$(CC) -o $@ $^ $(LDFLAGS)

lp.o: src/main.c
	$(CC) -c $(CFLAGS) -o $@ $<

lpfile.o: src/lpfile.c src/lpfile.h src/simplex.h src/shunt.h
	$(CC) -c $(CFLAGS) -o $@ $<

simplex.o: src/simplex.c src/simplex.h
	$(CC) -c $(CFLAGS) -o $@ $<

shunt.o: src/shunt.c src/shunt.h
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -rf lp.o lpfile.o simplex.o shunt.o $(BIN)

run: all
	./$(BIN) $(ARG)

runfull: all
	./$(BIN) $(ARG) $(ARG2) $(ARG3)
