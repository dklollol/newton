
# The ersp package config file must be in the PKG_CONFIG_PATH, e.g.
# export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/opt/evolution_robotics/lib/pkgconfig

# Compile and link flags
CXX      =  g++
CXXFLAGS = `pkg-config --cflags playerc++` -Wall
LIBS     = `pkg-config --libs playerc++`

# Main executable
EXECUTABLE = main

# Files
SOURCES = main.cc
OBJECTS = $(patsubst %.cc, %.o, $(SOURCES))

## Compile everything ##
all: $(OBJECTS)
	$(CXX) $(INCLUDES) $(OBJECTS) $(LIBS) -o $(EXECUTABLE)

# Compilation.
%.o : %.cc
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $^ 

# Cleaning up
clean :
	rm -f *.o *.lo *.la *.so *~ $(EXECUTABLE)
