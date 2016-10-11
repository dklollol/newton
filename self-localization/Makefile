# Compile and link flags
CXX         =  g++
CXXFLAGS    = -Wall
OPENCVFLAGS = `pkg-config opencv --cflags`
CXXFLAGS    = $(OPENCVFLAGS) -Wall
OPENCVLIBS  = `pkg-config opencv --libs`

# Main executable
EXECUTABLE = exercise5

# Files
SOURCES = particles.cc random_numbers.cc camera.cc
CCFILES = $(EXECUTABLE).cc $(SOURCES)
OBJECTS = $(patsubst %.cc, %.o, $(CCFILES))

## Compile everything ##
all: $(EXECUTABLE)

# Linking
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OPENCVLIBS) $(OBJECTS) -o $(EXECUTABLE)

# Compilation.
%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $^ 

# Cleaning up
clean :
	rm -f *.o $(EXECUTABLE)
