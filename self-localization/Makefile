# Compile and link flags
CXX = g++
PKGFLAGS = `pkg-config opencv playerc++ --cflags`
IGNORE_LIBPLAYER_JUNK_FLAGS = -Wno-cpp -Wno-unused-parameter -Wno-narrowing
CXXFLAGS = -Wall $(PKGFLAGS) $(IGNORE_LIBPLAYER_JUNK_FLAGS)
PKGLIBS = `pkg-config opencv playerc++ --libs`

# Main executable
EXECUTABLE = exercise5

# Files
SOURCES = particles.cc random_numbers.cc camera.cc robot.cc draw.cc misc.cc
CCFILES = $(EXECUTABLE).cc $(SOURCES)
OBJECTS = $(patsubst %.cc, %.o, $(CCFILES))

# Compile everything
all: $(EXECUTABLE)

# Linking
$(EXECUTABLE): $(OBJECTS)
	$(CXX) -o $(EXECUTABLE) $(OBJECTS) $(PKGLIBS)

# Compilation
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $^

# Cleaning up
clean:
	rm -f *.o $(EXECUTABLE)
