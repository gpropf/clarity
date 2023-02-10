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

cl2.a: ENV = 
cl2.a: WebElement.o Signal.o
	rm -f cl2.a
	$(ENV) emar --format=default -crs cl2.a WebElement.o Signal.o
	emranlib cl2.a

signalTest: signalTest.o #WebElement.o Signal.o 
	$(ENV) $(CC) $(CFLAGS) -lembind signalTest.o -o signalTest.js
	#$(ENV) $(CC) $(CFLAGS) -lembind --pre-js WebElement.js signalTest.o -o signalTest.js
# docs: clarity.doxyconfig
# 	doxygen clarity.doxyconfig

# docsclean:
# 	rm -rf html latex

# clean:
# 	rm -f *.o clarity_embind.js

simpleTest: simpleTest.o #WebElement.o Signal.o 
	$(ENV) $(CC) $(CFLAGS) -lembind simpleTest.o -o signalTest.js



realclean:
	rm -f *.o *.wasm *.wasm.map *.a signalTest.js

# all: showcase speedtest docs