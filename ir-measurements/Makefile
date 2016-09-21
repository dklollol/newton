.PHONY: all clean

# Misc. flags
IGNORE_LIBPLAYER_JUNK_FLAGS = -Wno-cpp -Wno-unused-parameter -Wno-narrowing

# Compiler and link flags
CXX = g++
CXXFLAGS = `pkg-config --cflags playerc++`
CXXFLAGS += -Wall -Wextra
CXXFLAGS += $(IGNORE_LIBPLAYER_JUNK_FLAGS)
LIBS = `pkg-config --libs playerc++`

# Executables
EXECUTABLE = main

# Files
SOURCES = main.cc

# Compilation
all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LIBS) -o $(EXECUTABLE)

clean:
	rm -f *.o $(EXECUTABLE)
