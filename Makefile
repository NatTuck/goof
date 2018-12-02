
EI_GUESS := /usr/lib/erlang/usr/include
GUESS_OK := $(shell bash -c "[[ -d $(EI_GUESS) ]] && echo ok")

ifeq "ok" "$(GUESS_OK)"
	ERLINC := $(EI_GUESS)
else
	ERLINC := $(shell dirname `find /usr -name "erl_nif.h" | head -n 1`)
endif

CFLAGS  := -c -fPIC -I$(ERLINC) -D_GNU_SOURCE `pkg-config --cflags glfw3`
LDFLAGS := -shared
LDLIBS  := `pkg-config --libs glfw3`

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)

priv/goof.so: $(OBJS)
	mkdir -p priv
	gcc $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

src/%.o: $(SRCS)
	gcc $(CFLAGS) -o $@ $<

clean:
	rm -f priv/*.so src/*.o

.PHONY: clean

