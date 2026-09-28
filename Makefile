CC = cc
CLANG = clang
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic
SDL_CFLAGS := $(shell pkg-config --cflags sdl2)
SDL_LIBS := $(shell pkg-config --libs sdl2)

.PHONY: all clean ir

all: turmites

turmites: app.c sim.c start.c sim.h
	$(CC) $(CFLAGS) $(SDL_CFLAGS) app.c sim.c start.c $(SDL_LIBS) -o $@

ir: app-opt.ll

app-opt.ll: app.c sim.h
	$(CLANG) -std=c11 -O2 -S -emit-llvm app.c -o $@
	sed -i '/^;/d' $@

clean:
	rm -f turmites
