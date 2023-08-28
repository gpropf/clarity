AR 		= emar
CC		= emcc

#ENV		= DEMANGLE_SUPPORT=1 EMCC_DEBUG=1
ENV		= DEMANGLE_SUPPORT=1

#CFLAGS	=  -O0 -g -gsource-map --source-map-base=http://localhost:6931/ -std=c++17
#DEBUG_CFLAGS = -O0 -g -std=c++17 -sALLOW_MEMORY_GROWTH -fsanitize=undefined -s ASSERTIONS=2 -s SAFE_HEAP=1 -s STACK_OVERFLOW_CHECK=1 -s WASM=1
DEBUG_CFLAGS = -O0 -g -std=c++17 -sALLOW_MEMORY_GROWTH
CFLAGS	= $(DEBUG_CFLAGS)

#CFLAGS	=  -O3 -std=c++17


%.o : %.cpp
	$(ENV) $(CC) $(CFLAGS) $< -o $@ -c

# %.o : %.hpp
# 	$(ENV) $(CC) $(CFLAGS) $< -o $@ -c

all: signalTest
	make -C examples

cl2.a: ENV = 
cl2.a: WebElement.o Signal.o
	rm -f cl2.a
	$(ENV) emar --format=default -crs cl2.a WebElement.o Signal.o
	emranlib cl2.a

signalTest: signalTest.o #WebElement.o Signal.o 
	$(ENV) $(CC) $(CFLAGS) -lembind signalTest.o -o signalTest.js
	

# docsclean:
# 	rm -rf html latex

# clean:
# 	rm -f *.o clarity_embind.js

clean:	
	rm -f *.o *.wasm *.wasm.map *.a signalTest.js

simpleTest: simpleTest.o #WebElement.o Signal.o 
	$(ENV) $(CC) $(CFLAGS) -lembind simpleTest.o -o signalTest.js



realclean:
	rm -f *.o *.wasm *.wasm.map *.a signalTest.js

