AR 		= emar
CC		= emcc
ENV		= DEMANGLE_SUPPORT=1

DEBUG_CFLAGS = -O0 -g -std=c++17 -sALLOW_MEMORY_GROWTH -I../../
CFLAGS	= $(DEBUG_CFLAGS)

%.o : %.cpp
	$(ENV) $(CC) $(CFLAGS) $< -o $@ -c


pixelreactor: pixelreactor.o
	$(ENV) $(CC) $(CFLAGS) -lembind pixelreactor.o -o pixelreactor.js


clean:	
	rm -f *.o *.wasm *.wasm.map *.a pixelreactor.js