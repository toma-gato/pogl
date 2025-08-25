TARGET = pogl

SRCS = $(wildcard src/*.cc)

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
LDFLAGS = -lGL -lGLEW -lglfw -lm

all: $(TARGET)

$(TARGET): $(SRCS)
		$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
		rm -f $(TARGET)

.PHONY: all clean