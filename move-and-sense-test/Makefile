.PHONY: all clean

# Misc. flags
IGNORE_LIBPLAYER_JUNK_FLAGS = -Wno-cpp -Wno-unused-parameter -Wno-narrowing
STAGE_FLAGS = -DUSING_STAGE

# Compiler and link flags
CXX = g++
CXXFLAGS = `pkg-config --cflags playerc++`
CXXFLAGS += -Wall -Wextra
CXXFLAGS += $(IGNORE_LIBPLAYER_JUNK_FLAGS)
LIBS = `pkg-config --libs playerc++`

# Executables
EXECUTABLE = main
EXECUTABLE_STAGE = main-stage

# Files
SOURCES = main.cc sleep.hpp

# Compilation
all: $(EXECUTABLE) $(EXECUTABLE_STAGE)

$(EXECUTABLE): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LIBS) -o $(EXECUTABLE)

$(EXECUTABLE_STAGE): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(STAGE_FLAGS) $(SOURCES) $(LIBS) -o $(EXECUTABLE_STAGE)

clean:
	rm -f *.o $(EXECUTABLE) $(EXECUTABLE_STAGE)
