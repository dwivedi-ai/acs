CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -O2
CPPFLAGS += -Iinclude

SRCS   := $(wildcard src/*.cpp)
HEADERS := $(wildcard include/*.h)
TARGET := acs

.PHONY: all test update-tests clean

all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SRCS) -o $@

test: $(TARGET)
	./tests/run_tests.sh

# Regenerate every .expected file from the compiler's current output.
update-tests: $(TARGET)
	./tests/run_tests.sh --update

clean:
	rm -f $(TARGET)
	rm -rf build
