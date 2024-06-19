CXX = g++
CXXFLAGS = -std=c++11 `pkg-config --cflags libcamera`
LDFLAGS = `pkg-config --libs libcamera`

all: my_libcamera_app

my_libcamera_app: main.o
	$(CXX) -o $@ $^ $(LDFLAGS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f my_libcamera_app main.o