CXX = clang++
CXXFLAGS = -std=c++26 -Wall -Wextra -Wpedantic -Wconversion -Werror -O3 -fsanitize=thread

TARGETS = release-sequence shared-ptr-thread-args-copied
SOURCES_REL_SEQ = release-sequence.cc
SOURCES_SHARE_PTR = shared-ptr-thread-args-copied.cc

all: $(TARGETS)

release-sequence: $(SOURCES_REL_SEQ)
	$(CXX) -o $@ $(SOURCES_REL_SEQ) $(CXXFLAGS)

shared-ptr-thread-args-copied: $(SOURCES_SHARE_PTR)
	$(CXX) -o $@ $(SOURCES_SHARE_PTR) $(CXXFLAGS)

clean:
	rm -f $(TARGETS)

.PHONY: clean all
