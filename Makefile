NAME ?= polyadvent
BIN ?= $(NAME)
PREFIX ?= /usr/local
DEFS= -DGLFW_INCLUDE_NONE -DDEBUG
CFLAGS = $(DEFS) -ggdb -O2 -I src -Wall -Werror -Wextra -std=c99 \
						-Wno-unused-function \
						-Wno-unused-parameter \
						-Wno-unused-variable \
						-Wno-cast-align \
						-Wno-padded
LDFLAGS = -lSDL2 -lGL -lm
SRC=src

OBJS  = $(SRC)/window.o
OBJS += $(SRC)/buffer.o
OBJS += $(SRC)/camera.o
OBJS += $(SRC)/xml.o
OBJS += $(SRC)/animation.o
OBJS += $(SRC)/debug.o
OBJS += $(SRC)/delaunay.o
OBJS += $(SRC)/entity.o
OBJS += $(SRC)/fbo.o
OBJS += $(SRC)/file.o
OBJS += $(SRC)/game.o
OBJS += $(SRC)/geometry.o
OBJS += $(SRC)/hires.o
OBJS += $(SRC)/input.o
OBJS += $(SRC)/mat4.o
OBJS += $(SRC)/mat_util.o
OBJS += $(SRC)/model.o
OBJS += $(SRC)/node.o
OBJS += $(SRC)/orbit.o
OBJS += $(SRC)/perlin.o
OBJS += $(SRC)/ply.o
OBJS += $(SRC)/poisson.o
OBJS += $(SRC)/quat.o
OBJS += $(SRC)/render.o
OBJS += $(SRC)/shader.o
OBJS += $(SRC)/skybox.o
OBJS += $(SRC)/stb_image.o
OBJS += $(SRC)/terrain.o
OBJS += $(SRC)/texture.o
OBJS += $(SRC)/ui.o
OBJS += $(SRC)/uniform.o
OBJS += $(SRC)/update.o
OBJS += $(SRC)/util.o
OBJS += $(SRC)/vec3.o
OBJS += $(SRC)/quickhull.o

TESTS = test/test_animation

SRCS=$(OBJS:.o=.c)

all: $(BIN)

include $(OBJS:.o=.d)
include src/main.d
include test/test_animation.d

%.d: %.c
	@rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\(.*\)\.o[ :]*,src/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

test/%: test/%.o $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

check: $(TESTS)
	./$(TESTS)

$(BIN): src/main.o $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

install: $(BIN)
	install -d $(PREFIX)/bin
	install $(BIN) $(PREFIX)/bin

nixbuild:
	nix-shell shell.nix --command 'make -j4'

TAGS:
	etags $(SRCS)

clean:
	rm -f src/main.o $(OBJS) $(TESTS) $(SHLIB) $(BIN) $(SRC)/*.d*

.PHONY: TAGS
