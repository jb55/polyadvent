
BIN ?= polycraft
PREFIX ?= /usr/local
LDFLAGS = -lglfw3 -lX11 -lGL -lXi -lXrandr -lXxf86vm

all: $(BIN)

%.o: %.c
	$(CXX) -o $@ -c $<

$(BIN): $(OBJS)
	$(CXX) $^ main.cc $(LDFLAGS) -o $@

install: $(BIN)
	install -d $(PREFIX)/bin
	install $(BIN) $(PREFIX)/bin

nixbuild:
	nix-build shell.nix

clean:
	find . -name '*.o' -exec rm -f {} \;
	rm -f csv
