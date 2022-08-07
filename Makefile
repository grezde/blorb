
FLAGS := 
SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c,%.o,$(SOURCES))
OBJECTSREL := $(patsubst src/%.c,temp/%.o,$(SOURCES))

FLAGSPP :=
SOURCESPP := $(wildcard srcpp/*.cpp)
OBJECTSPP := $(patsubst srcpp/%.cpp,pp/%.o,$(SOURCESPP))
OBJECTSRELPP := $(patsubst srcpp/%.cpp,temppp/%.o,$(SOURCESPP))


%.o: src/%.c
	gcc $(FLAGS) -Isrc/include -c $< -o temp/$@

pp/%.o: srcpp/%.cpp
	g++ $(FLAGSPP) -Isrcpp/include -c $< -o temp$@

maketest:
	echo $(OBJECTS)
	echo $(OBJECTSPP)

build: $(OBJECTS)
	gcc $(OBJECTSREL) $(FLAGS) -o blorbc  

buildpp: $(OBJECTSPP)
	g++ $(OBJECTSRELPP) $(FLAGSPP) -o blorbc

runpp: buildpp
	./blorbc test.b

clean:
	rm -rf temp
	rm -rf temppp
	mkdir temp
	mkdir temppp

run: build
	./blorbc test.b