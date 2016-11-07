.PHONY: all clean

CXX = g++
CXXFLAGS = -Wall -Wextra
IGNORE_LIBPLAYER_JUNK_FLAGS = -Wno-cpp -Wno-unused-parameter -Wno-narrowing
CXXFLAGS_PLAYER = `pkg-config --cflags playerc++`
CXXFLAGS_PLAYER += $(IGNORE_LIBPLAYER_JUNK_FLAGS)
CXXFLAGS_OPENCV = `pkg-config --cflags opencv`
LIBS_PLAYER = `pkg-config --libs playerc++`
LIBS_OPENCV = `pkg-config --libs opencv`

all: camtest

camtest: cam.o camtest.o
	$(CXX) cam.o camtest.o $(LIBS_OPENCV) -o camtest

cam.o: cam.cc
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_OPENCV) -c $^

camtest.o: camtest.cc
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_OPENCV) -c $^

clean:
	rm -f *.o camtest findfov main
