CC = g++
CXXFLAGS = -g -Wall -std=c++11 -Iinclude -I/usr/include/freetype2
LDFLAGS = -g -Wall

CPP_FILES = $(wildcard src/*.cpp)
OBJ_FILES = $(subst src/,obj/,$(subst .cpp,.o,$(CPP_FILES)))
LIB_FILE = lib/libfont2svg.a

CPP_FILES_TOOLS = $(wildcard tools/*.cpp)
BIN_FILES_TOOLS = $(subst tools/,bin/,$(subst .cpp,,$(CPP_FILES_TOOLS)))
  
all: dirs $(BIN_FILES_TOOLS)

clean:
	rm -rf bin lib obj

dirs:
	mkdir -p bin lib obj/tools



bin/%: obj/tools/%.o $(LIB_FILE)
	$(CC) $(LDFLAGS) -Llib -o $@ $< -lfont2svg -lfreetype

obj/tools/%.o: tools/%.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $<



$(LIB_FILE): $(OBJ_FILES)
	ar rcs $@ $^

obj/%.o: src/%.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $<



.PHONY: all clean dirs
