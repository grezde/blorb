
FLAGS := 
SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c,%.o,$(SOURCES))
OBJECTSREL := $(patsubst src/%.c,temp/%.o,$(SOURCES))

%.o: src/%.c
	gcc -c $(FLAGS) -Isrc/include $< -o temp/$@

maketest:
	echo $(OBJECTS)

build: $(OBJECTS)
	gcc $(OBJECTSREL) $(FLAGS) -o blorbc  

run: build
	./blorbc test.b
	cat test.b | bc