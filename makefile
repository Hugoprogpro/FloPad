
CC = g++

CFLAGS = -Wall -O2 `wx-config --cflags std stc` `pkg-config sqlite3 --cflags`
LDFLAGS = `wx-config --libs std stc` `pkg-config sqlite3 --libs`

COMPILE = $(CC) $(CFLAGS) -c

OBJFILES := $(patsubst %.cpp,%.o,$(wildcard src/ftp/*.cpp)) $(patsubst %.cpp,%.o,$(wildcard src/*.cpp))



all: flopad



flopad: $(OBJFILES)

	$(CC) $(LDFLAGS) -o flopad $(OBJFILES)



%.o: %.cpp
	$(COMPILE) -o $@ $<

clean:
	rm -f flopad
	rm -f src/*.o
	rm -f src/ftp/*.o


