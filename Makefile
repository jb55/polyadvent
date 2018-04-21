NAME ?= polyadvent
BIN ?= $(NAME)
PREFIX ?= /usr/local
CFLAGS = -ggdb -I src -Wall -Wextra -Werror -std=c99 \
						-Wno-unused-function \
						-Wno-unused-parameter \
						-Wno-unused-variable \
						-Wno-cast-align \
						-Wno-padded
LDFLAGS = -lSDL2 -lGL
DEFS= -DGLFW_INCLUDE_NONE
SRC=src

SHLIB=$(SRC)/lib$(NAME).so

OBJS  = $(SRC)/window.o
OBJS += $(SRC)/buffer.o
#OBJS += $(SRC)/camera.o
OBJS += $(SRC)/debug.o
OBJS += $(SRC)/event.o
OBJS += $(SRC)/file.o
OBJS += $(SRC)/game.o
OBJS += $(SRC)/mat4/mat4.o
OBJS += $(SRC)/render.o
OBJS += $(SRC)/shader.o
OBJS += $(SRC)/update.o
OBJS += $(SRC)/slab.o
OBJS += $(SRC)/slab_geom.o
OBJS += $(SRC)/geometry.o
OBJS += $(SRC)/util.o

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
	nix-shell shell.nix --command 'make -j4'

clean:
	rm -f src/main.o $(OBJS) $(SHLIB) $(BIN)
