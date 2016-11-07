# Flags
CXX = g++
PKGFLAGS = `pkg-config opencv playerc++ --cflags`
IGNORE_LIBPLAYER_JUNK_FLAGS = -Wno-cpp -Wno-unused-parameter -Wno-narrowing
CXXFLAGS = -std=c++11 -Wall $(PKGFLAGS) $(IGNORE_LIBPLAYER_JUNK_FLAGS)
PKGLIBS = -lpthread `pkg-config opencv playerc++ --libs`

# Sources
EXECUTABLE = main
SOURCES = particles.cc random_numbers.cc camera.cc robot.cc draw.cc misc.cc strategy.cc timing.cc green-partition/cam.cc
CCFILES = $(EXECUTABLE).cc $(SOURCES)
OBJECTS = $(patsubst %.cc, %.o, $(CCFILES))

# Compile everything
all: $(EXECUTABLE)

# Link
$(EXECUTABLE): $(OBJECTS)
	$(CXX) -o $(EXECUTABLE) $(OBJECTS) $(PKGLIBS)

# Compile
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $(patsubst %.o, %.cc, $<)

# Clean
clean:
	rm -f *.o $(EXECUTABLE)
