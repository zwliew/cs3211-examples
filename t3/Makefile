CXX = clang++
CXXFLAGS = -std=c++26 -Wall -Wextra -Wpedantic -Wconversion -Werror -O3 -fsanitize=thread

TARGETS = release-sequence shared-ptr-thread-args-copied shared-ptr-alt-dtor
SOURCES_REL_SEQ = release-sequence.cc
SOURCES_SHARED_PTR = shared-ptr-thread-args-copied.cc
SOURCES_SHARED_PTR_ALT_DTOR = shared-ptr-alt-dtor.cc

all: $(TARGETS)

release-sequence: $(SOURCES_REL_SEQ)
	$(CXX) -o $@ $(SOURCES_REL_SEQ) $(CXXFLAGS)

shared-ptr-thread-args-copied: $(SOURCES_SHARED_PTR)
	$(CXX) -o $@ $(SOURCES_SHARED_PTR) $(CXXFLAGS)

shared-ptr-alt-dtor: $(SOURCES_SHARED_PTR_ALT_DTOR)
	$(CXX) -o $@ $(SOURCES_SHARED_PTR_ALT_DTOR) $(CXXFLAGS)

clean:
	rm -f $(TARGETS)

.PHONY: clean all
