CXX = clang++
CXXFLAGS = -std=c++11 -Xpreprocesser -fopenmp
LDFLAGS = -L/usr/local/opt/libomp/lib -lomp
CPPFLAGS = -I/usr/local/opt/libomp/include
TARGET = raytracer
SOURCES = src/main.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS) $(CPPFLAGS)

clean:
	rm -f $(TARGET)

