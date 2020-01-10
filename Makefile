NAME ?= polyadvent
BIN ?= $(NAME)
PREFIX ?= /usr/local
# DEFS= -DGLFW_INCLUDE_NONE -DDEBUG
# release build lol
# DEFS= -DGLFW_INCLUDE_NONE -DNDEBUG

# CFLAGS = $(DEFS) -ggdb -O0 -I src -Wall -Wextra -std=c99 \

CFLAGS = $(DEFS) -Ofast -DSDL_DISABLE_IMMINTRIN_H \
	$(shell pkg-config --cflags sdl2 gl x11) \
	-DSTBI_NO_SIMD \
	-I src -Wall -Werror -Wextra -std=c99 \
	-Wno-unused-function \
	-Wno-unused-parameter \
	-Wno-unused-variable \
	-Wmissing-field-initializers \
	-Wno-cast-align \
	-Wno-padded

LDFLAGS = $(shell pkg-config --libs-only-L gl xcb xi xau xdmcp xext xcursor xrender xrandr xfixes xinerama xscrnsaver xxf86vm) $(shell pkg-config --libs sdl2) -lGL -lm
SRC=src

SRCS=$(wildcard $(SRC)/*.c)
PLYS=$(wildcard data/models/*.ply)

MODELS=$(PLYS:.ply=.mdl)
OBJS=$(SRCS:.c=.o)

TESTS =  test/test_dae
TESTS += test/test_resource
TESTS += test/test_scene

TOOLS = tools/compile-model

all: tools $(BIN) $(MODELS)

clean:
	rm -f src/main.o test/*.o tools/*.o polyadvent.o $(TESTS) $(TOOLS) $(MODELS) $(SHLIB) $(BIN) $(SRC)/*.d*

polyadvent.o: $(SRCS)
	@echo "cc $@"
	@$(CC) $(CFLAGS) -r -o $@ $^

test/%: test/%.o polyadvent.o
	@echo "link $@"
	@$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

tools/%: tools/%.c polyadvent.o
	@echo "link $@"
	@$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

tools: $(TOOLS)

data/models/%.mdl: data/models/%.ply tools/compile-model
	@echo "compile-model $@"
	@./tools/compile-model $< $@

check: $(TESTS) $(MODELS)
	./test/test_dae
	./test/test_resource
	./test/test_scene

$(BIN): polyadvent.o main.o
	@echo "link $@"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

install: $(BIN)
	install -d $(PREFIX)/bin
	install $(BIN) $(PREFIX)/bin

TAGS:
	etags $(SRCS)

.PHONY: TAGS
