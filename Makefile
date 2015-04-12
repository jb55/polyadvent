NAME ?= polyadvent
BIN ?= $(NAME)
PREFIX ?= /usr/local
CFLAGS = -ggdb -Wall -Wextra -Weverything -Werror
LDFLAGS = -lSDL2 -lGL
DEFS= -DGLFW_INCLUDE_NONE
SRC=src

SHLIB=$(SRC)/lib$(NAME).so

OBJS  = $(SRC)/window.o
OBJS += $(SRC)/buffer.o
OBJS += $(SRC)/update.o
OBJS += $(SRC)/event.o
OBJS += $(SRC)/shader.o
OBJS += $(SRC)/file.o
OBJS += $(SRC)/debug.o
OBJS += $(SRC)/render.o

all: $(BIN)

%.o: %.c %.h
	$(CC) $(CFLAGS) -fPIC $(DEFS) -c $< -o $@

$(SHLIB): $(OBJS)
	$(CC) $(CFLAGS) -shared $^ -o $@

$(BIN): $(SRC)/main.o $(SHLIB)
	$(CC) $(CFLAGS) $(DEFS) $^ $(LDFLAGS) -o $@

install: $(BIN)
	install -d $(PREFIX)/bin
	install $(BIN) $(PREFIX)/bin

nixbuild:
	nix-shell shell.nix --pure --command 'make -j4'

clean:
	rm -f main.o $(OBJS) $(SHLIB) $(BIN)
