# Simple makefile for the terminal StarCraft clone
# Requires ncurses development package (libncurses5-dev or libncursesw5-dev)

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -lncurses -lform -lpanel -lm
SRC = src/main.cpp
TARGET = starcraft

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

build:
	make

build-js:
	@echo 'Building JavaScript assets...'
	webpack --mode production

clean:
	@echo 'Cleaning up...'
	rm -f $(TARGET) dist/*.js
