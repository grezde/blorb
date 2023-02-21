
FLAGS := -Wno-delete-incomplete
SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(patsubst src/%.cpp,%.o,$(SOURCES))
OBJECTSREL := $(patsubst src/%.cpp,temp/%.o,$(SOURCES))

%.o: src/%.cpp
	gcc $(FLAGS) -Isrc/include -c $< -o temp/$@

build: $(OBJECTS)
	g++ $(OBJECTSREL) $(FLAGS) -o blorbc  

clean:
	rm -rf temp

run-%: examples/%.txt
	./blorbc $<

build-%: examples/%.txt
	clear
	make build
	./blorbc $<

loc:
	printf 'Lines of code: ' ; cat src/*.cpp src/include/*.hpp | wc -l