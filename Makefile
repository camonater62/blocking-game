MAIN = main.cpp
OUT = blocking

CC = clang++
WCC = x86_64-w64-mingw32-g++
EMCC = em++

CFLAGS = -O2 -std=c++17 
WFLAGS = -O2 -std=c++17 -static
EMFLAGS = -std=c++17 -O2 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 -s USE_LIBPNG=1 -s USE_SDL_MIXER=2

LINKS = -lX11 -lGL -lpthread -lpng -lpulse -lpulse-simple
WLINKS = -luser32 -lgdi32 -lopengl32 -lgdiplus -lshlwapi -ldwmapi -lstdc++fs -lwinmm

default: $(MAIN)
	$(CC) $(CFLAGS) -o $(OUT) $(MAIN) $(LINKS)

windows: $(MAIN)
	$(WCC) $(WFLAGS) -o $(OUT).exe $(MAIN) $(WLINKS)

web: 
	$(EMCC) $(EMFLAGS) -o $(OUT).html $(MAIN)

clean:
	rm -f $(OUT) $(OUT).exe *.o $(OUT).html $(OUT).js $(OUT).wasm

scan-build: clean
	scan-build make

format:
	clang-format -i *.cpp *.hpp

run: default
	devour ./$(OUT)
