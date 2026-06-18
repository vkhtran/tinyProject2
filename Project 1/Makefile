CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude

SRC = src/Vector.cpp \
      src/Matrix.cpp \
      src/LinearSystem.cpp \
      src/PosSymLinSystem.cpp \
      src/LinearRegression.cpp \
      src/main.cpp

TARGET = project1

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET) data/machine.data

.PHONY: all clean run
