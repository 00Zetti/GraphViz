CXXFLAGS= -std=c++98 -Wall -g
LIBS= 
TARGET= graphviz
OBJS= src/graphviz.o \
      src/compound.o

all: $(TARGET)

run: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(CXXFLAGS) $(LIBS)

src/graphviz.o: src/graphviz.cpp
	$(CXX) $< -c -o $@ $(CXXFLAGS)

src/%.o: src/%.cpp src/%.h
	$(CXX) $< -c -o $@ $(CXXFLAGS)

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all run clean
