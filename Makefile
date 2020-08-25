$(shell CC="$(CC)" CXX="$(CXX)" TARGET_OS="$(TARGET_OS)" ./build_config 1>&2)
include config.mk

DEBUG=n
CONV=n

CFLAGS	:= -std=c++17 -Wall

ifeq ($(DEBUG),y)
	CFLAGS += -g2 -O0
else
	CFLAGS += -O3 -DNDEBUG
endif

ifeq ($(CONV),y)
	INCLUDE+= -I /usr/local/libiconv/include
	LIBS+= -L /usr/local/libiconv/lib

	CFLAGS += -fprofile-arcs -ftest-coverage
	CFLAGS += -lgcov
endif

CFLAGS += -I. $(PLATFORM_CCFLAGS) 
CXXFLAGS += -I. $(PLATFORM_CXXFLAGS) 

LDFLAGS += $(PLATFORM_LDFLAGS)
LIBS += $(PLATFORM_LIBS)

HANDY_SOURCES += $(shell find handy -name '*.cc')
HANDY_OBJECTS = $(HANDY_SOURCES:.cc=.o)

TEST_SOURCES = $(shell find test -name '*.cc')
TEST_OBJECTS = $(TEST_SOURCES:.cc=.o)

EXAMPLE_SOURCES += $(shell find examples -name '*.cc')
EXAMPLES = $(EXAMPLE_SOURCES:.cc=)

KW_SOURCES += $(shell find 10m -name '*.cc')
KW = $(KW_SOURCES:.cc=)

LIBRARY = libhandy.a

TARGETS = $(LIBRARY) handy_test $(EXAMPLES) $(KW)

default: $(TARGETS)
handy_examples: $(EXAMPLES)
$(EXAMPLES): $(LIBRARY)
$(KW): $(LIBRARY)

PREFIX=/usr/local/handy

install: libhandy.a
	mkdir -p ${PREFIX}/include/handy
	mkdir -p ${PREFIX}/lib
	cp -f handy/*.h ${PREFIX}/include/handy/
	cp -f libhandy.a $(PREFIX)/lib/

uninstall:
	rm -rf $(PREFIX)

clean:
			-rm -f $(TARGETS)
			-rm -f */*.o

$(LIBRARY): $(HANDY_OBJECTS)
		rm -f $@
			$(AR) -rs $@ $(HANDY_OBJECTS)

handy_test: $(TEST_OBJECTS) $(LIBRARY)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS)

.cc.o:
		$(CXX) $(CXXFLAGS) -c $< -o $@

.c.o:
		$(CC) $(CFLAGS) -c $< -o $@

.cc:
	$(CXX) -o $@ $< $(CXXFLAGS) $(LDFLAGS) $(LIBRARY) $(LIBS)
