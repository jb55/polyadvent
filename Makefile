NAME ?= polyadvent
BIN ?= $(NAME)
PREFIX ?= /usr/local
CFLAGS = -ggdb -I src -Wall -Werror -Wextra -std=c99 \
						-Wno-unused-function \
						-Wno-unused-parameter \
						-Wno-unused-variable \
						-Wno-cast-align \
						-Wno-padded
LDFLAGS = -lSDL2 -lGL
DEFS= -DGLFW_INCLUDE_NONE
SRC=src

OBJS  = $(SRC)/window.o
OBJS += $(SRC)/buffer.o
OBJS += $(SRC)/camera.o
OBJS += $(SRC)/debug.o
OBJS += $(SRC)/event.o
OBJS += $(SRC)/file.o
OBJS += $(SRC)/perlin.o
OBJS += $(SRC)/main.o
OBJS += $(SRC)/poisson.o
OBJS += $(SRC)/uniform.o
OBJS += $(SRC)/game.o
OBJS += $(SRC)/mat4.o
OBJS += $(SRC)/vec3.o
OBJS += $(SRC)/render.o
OBJS += $(SRC)/shader.o
OBJS += $(SRC)/update.o
OBJS += $(SRC)/terrain.o
OBJS += $(SRC)/slab.o
OBJS += $(SRC)/slab_geom.o
OBJS += $(SRC)/delaunay.o
OBJS += $(SRC)/geometry.o
OBJS += $(SRC)/input.o
OBJS += $(SRC)/util.o
OBJS += $(SRC)/node.o

SRCS=$(OBJS:.o=.c)


all: $(BIN)

include $(OBJS:.o=.d)

%.d: %.c
	@rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\(.*\)\.o[ :]*,src/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(DEFS) $^ $(LDFLAGS) -o $@

install: $(BIN)
	install -d $(PREFIX)/bin
	install $(BIN) $(PREFIX)/bin

nixbuild:
	nix-shell shell.nix --command 'make -j4'

TAGS:
	etags $(SRCS)

clean:
	rm -f src/main.o $(OBJS) $(SHLIB) $(BIN) $(SRC)/*.d*

.PHONY: TAGS
