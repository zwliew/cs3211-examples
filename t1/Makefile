CXX = clang++
CXXFLAGS = -std=c++26 -Wall -Wextra -Wpedantic -Wconversion

TARGETS = logging-single logging-threaded
SOURCES_SINGLE = logging-single.cc
SOURCES_THREADED = logging-threaded.cc

all: $(TARGETS)

logging-single: $(SOURCES_SINGLE)
	$(CXX) -o $@ $(SOURCES_SINGLE) $(CXXFLAGS)

logging-threaded: $(SOURCES_THREADED)
	$(CXX) -o $@ $(SOURCES_THREADED) $(CXXFLAGS)

clean:
	rm -f $(TARGETS)

.PHONY: clean all
