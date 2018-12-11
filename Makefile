CPPFLAGS = -Wall
CPPFLAGS += $(shell pkg-config --cflags glu)
LDFLAGS += $(shell pkg-config --libs-only-L glu)
LDLIBS += $(shell pkg-config --libs-only-l glu)

BIN = eskimo

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=%.o)

.PHONY:	all clean

all:	$(BIN)

clean:
	$(RM) $(OBJS) $(BIN)

$(BIN):	$(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)
