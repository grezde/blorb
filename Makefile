
FLAGS := 
SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(patsubst src/%.cpp,%.o,$(SOURCES))
OBJECTSREL := $(patsubst src/%.cpp,temp/%.o,$(SOURCES))

%.o: src/%.cpp
	gcc $(FLAGS) -Isrc/include -c $< -o temp/$@

build: $(OBJECTS)
	g++ $(OBJECTSREL) $(FLAGS) -o blorbc  

clean:
	rm -rf temp

run:
	clear
	make build
	./blorbc examples/test.txt