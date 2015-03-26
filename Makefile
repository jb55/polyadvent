
BIN ?= polyadvent
PREFIX ?= /usr/local
LDFLAGS = -lglfw3 -lX11 -lGL -lXi -lXrandr -lXxf86vm -lepoxy
DEFS= -DGLFW_INCLUDE_NONE

OBJS  = render.o
OBJS += window.o
OBJS += buffer.o

all: $(BIN)

%.o: %.cc
	$(CXX) $(DEFS) -c $< -o $@

$(BIN): $(OBJS)
	$(CXX) $(DEFS) $^ main.cc $(LDFLAGS) -o $@

install: $(BIN)
	install -d $(PREFIX)/bin
	install $(BIN) $(PREFIX)/bin

nixbuild:
	nix-build shell.nix

clean:
	find . -name '*.o' -exec rm -f {} \;
	rm -f csv
