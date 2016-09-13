.PHONY: clean

# Compiler and link flags
CXX =  g++
IGNORE_LIBPLAYER_SLAM_FLAGS = -Wno-cpp -Wno-unused-parameter -Wno-narrowing
CXXFLAGS = `pkg-config --cflags playerc++` -Wall -Wextra $(IGNORE_LIBPLAYER_SLAM_FLAGS)
LIBS = `pkg-config --libs playerc++`

# Main executable
EXECUTABLE = abortmission

# Files
SOURCES = abortmission.cc
OBJECTS = $(patsubst %.cc, %.o, $(SOURCES))

## Compilation
all: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBS) -o $(EXECUTABLE)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $^

clean:
	rm -f *.o $(EXECUTABLE)
