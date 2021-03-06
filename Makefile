CC=gcc
CFLAGS=-g -Wall
SRC=src
OBJ=obj
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

BINDIR=bin
BIN=$(BINDIR)/galaga
SUBMITNAME=galaga_Mateus_Alves.zip

all: mkdirs $(BIN)

release: CFLAGS=-Wall -O2 -DNDEBUG
release: mkdirs clean
release: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

mkdirs:
	@mkdir -p $(OBJ)
	@mkdir -p $(BINDIR)

clean:
	$(RM) -r $(BINDIR)/* $(OBJ)/*

submit:
	$(RM) $(SUBMITNAME)
	zip $(SUBMITNAME) $(BIN) $(SRC)/* history.log Makefile
