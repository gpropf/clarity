AR 		= emar
CC		= emcc
ENV		= DEMANGLE_SUPPORT=1

DEBUG_CFLAGS = -O0 -g -std=c++17 -sALLOW_MEMORY_GROWTH -I../../
CFLAGS	= $(DEBUG_CFLAGS)

%.o : %.cpp
	$(ENV) $(CC) $(CFLAGS) $< -o $@ -c

# %.o : %.hpp
# 	$(ENV) $(CC) $(CFLAGS) $< -o $@ -c


matchsticks: matchsticks.o
	$(ENV) $(CC) $(CFLAGS) -lembind matchsticks.o -o matchsticks.js


# simpleTest: simpleTest.o #WebElement.o Signal.o 
# 	$(ENV) $(CC) $(CFLAGS) -lembind simpleTest.o -o signalTest.js


clean:	
	rm -f *.o *.wasm *.wasm.map *.a pixelreactor.js