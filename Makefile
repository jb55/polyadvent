
BIN ?= polyadvent
PREFIX ?= /usr/local
LDFLAGS = -lglfw3 -lX11 -lGL -lXi -lXrandr -lXxf86vm -lepoxy
DEFS= -DGLFW_INCLUDE_NONE
SRC=src

OBJS  = ${SRC}/main.o
OBJS += ${SRC}/window.o
OBJS += ${SRC}/buffer.o
OBJS += ${SRC}/shader.o
OBJS += ${SRC}/file.o
OBJS += ${SRC}/debug.o
OBJS += ${SRC}/render.o

all: $(BIN)

%.o: %.cc
	$(CXX) $(DEFS) -c $< -o $@

$(BIN): $(OBJS)
	$(CXX) $(DEFS) $^ $(LDFLAGS) -o $@

install: $(BIN)
	install -d $(PREFIX)/bin
	install $(BIN) $(PREFIX)/bin

nixbuild:
	nix-build shell.nix

clean:
	find . -name '*.o' -exec rm -f {} \;
	rm -f csv
