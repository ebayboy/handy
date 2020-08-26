include ../config.mk
CXXFLAGS += -g2 $(PLATFORM_CXXFLAGS) -lhandy $(PLATFORM_LDFLAGS)

SRCS=$(wildcard *.cc)
PROGS=$(SRCS:.cc=)

all:$(PROGS)
$(PROGS):%:%.cc
	$(CXX) $^ -o $@ $(CXXFLAGS)

.PHONY:clean
clean:
	rm -f $(PROGS)


