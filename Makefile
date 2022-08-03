
FLAGS := 
SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c,%.o,$(SOURCES))
OBJECTSREL := $(patsubst src/%.c,temp/%.o,$(SOURCES))

%.o: src/%.c
	gcc -c $(FLAGS) -Isrc/include $< -o $@
	mv $@ temp

test:
	echo $(OBJECTS)

build: $(OBJECTS)
	gcc $(OBJECTSREL) $(FLAGS) -o blorbc  