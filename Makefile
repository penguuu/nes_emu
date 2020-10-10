CC=gcc
CFLAGS=-c -Wall -fPIC -O3
LDFLAGS=-lSDL
SOURCES=cartridge.c cpu.c debug.c mem.c nes.c ppu.c render.c joystick.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=nes

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o nes
