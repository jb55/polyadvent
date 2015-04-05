NAME ?= polyadvent
BIN ?= $(NAME)
PREFIX ?= /usr/local
LDFLAGS = -lglfw3 -lX11 -lGL -lXi -lXrandr -lXxf86vm -lepoxy
DEFS= -DGLFW_INCLUDE_NONE
SRC=src

SHLIB=$(SRC)/lib$(NAME).so

OBJS  = $(SRC)/window.o
OBJS += $(SRC)/buffer.o
OBJS += $(SRC)/shader.o
OBJS += $(SRC)/file.o
OBJS += $(SRC)/debug.o
OBJS += $(SRC)/render.o

all: $(BIN)

%.o: %.cc %.h
	$(CC) -fPIC $(DEFS) -c $< -o $@

$(SHLIB): $(OBJS)
	$(CC) -shared $^ -o $@

$(BIN): $(SHLIB) $(SRC)/main.o
	$(CC) $(DEFS) $^ $(LDFLAGS) -o $@

install: $(BIN)
	install -d $(PREFIX)/bin
	install $(BIN) $(PREFIX)/bin

nixbuild:
	nix-shell shell.nix --command 'make -j4'

clean:
	rm -rf $(OBJS) $(SHLIB) $(BIN)

