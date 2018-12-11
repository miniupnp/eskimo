CPPFLAGS = -Wall
CPPFLAGS += $(shell pkg-config --cflags glu)
LDFLAGS += $(shell pkg-config --libs-only-L glu)
LDLIBS += $(shell pkg-config --libs-only-l glu)

CPPFLAGS += -DUSE_SDL2
CPPFLAGS += $(shell sdl2-config --cflags)
LDLIBS += $(shell sdl2-config --libs)

BIN = eskimo

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=%.o)

.PHONY:	all clean

all:	$(BIN)

clean:
	$(RM) $(OBJS) $(BIN)

$(BIN):	$(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)
