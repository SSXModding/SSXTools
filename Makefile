CXX = g++ -std=c++17 -O2 -march=native
LDFLAGS = -s

OBJS = obj/TextureExtractor.o obj/main.o

all: bin/eagle

bin/:
	mkdir -p $@
	
obj/:
	mkdir -p $@

bin/eagle: bin/ obj/ $(OBJS)
	$(CXX) $(OBJS) -o $@
	
obj/%.o: src/%.cpp
	$(CXX) -c $< -o $@