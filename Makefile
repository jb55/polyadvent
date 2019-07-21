NAME ?= polyadvent
BIN ?= $(NAME)
PREFIX ?= /usr/local
DEFS= -DGLFW_INCLUDE_NONE -DDEBUG
# release build lol
#DEFS= -DGLFW_INCLUDE_NONE -DNDEBUG
# CFLAGS = $(DEFS) -ggdb -O0 -I src -Wall -Wextra -std=c99
CFLAGS = $(DEFS) -ggdb -O2 -I src -Wall -Werror -Wextra -std=c99  \
						-Wno-unused-function \
						-Wno-unused-parameter \
						-Wno-unused-variable \
						-Wmissing-field-initializers \
						-Wno-cast-align \
						-Wno-padded
LDFLAGS = -lSDL2 -lGL -lm
SRC=src

SRCS=$(wildcard $(SRC)/*.c)
PLYS=$(wildcard data/models/*.ply)

MODELS=$(PLYS:.ply=.mdl)
OBJS=$(SRCS:.c=.o)

TESTS =  test/test_dae
TESTS += test/test_resource
TESTS += test/test_scene

TOOLS = tools/compile-model

all: $(BIN) $(MODELS)

clean:
	rm -f src/main.o test/*.o tools/*.o $(OBJS) $(TESTS) $(TOOLS) $(MODELS) $(SHLIB) $(BIN) $(SRC)/*.d*

include $(OBJS:.o=.d)
include main.d
include test/test_dae.d

%.d: %.c
	@rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\(.*\)\.o[ :]*,src/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

test/%: test/%.o $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

tools/%: tools/%.o $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

tools: $(TOOLS)

data/models/%.mdl: data/models/%.ply tools/compile-model
	./tools/compile-model $< $@

check: $(TESTS) $(MODELS)
	./test/test_dae
	./test/test_resource
	./test/test_scene

$(BIN): main.o $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

install: $(BIN)
	install -d $(PREFIX)/bin
	install $(BIN) $(PREFIX)/bin

TAGS:
	etags $(SRCS)

.PHONY: TAGS
