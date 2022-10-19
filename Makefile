AR 		= emar
CC		= emcc

#ENV		= DEMANGLE_SUPPORT=1 EMCC_DEBUG=1
ENV		= DEMANGLE_SUPPORT=1

#CFLAGS	=  -O0 -g -gsource-map --source-map-base=http://localhost:6931/ -std=c++17
CFLAGS	=  -O0 -g -std=c++17 #-sASSERTIONS
#CFLAGS	=  -O3 -std=c++17

JSOUT	= clarity_embind.js

LIBCLR_IN = ClarityNode.cpp ModelNode.hpp CanvasElement.cpp CLNodeFactory.hpp			
LIBCLR_OBJS = ClarityNode.o ModelNode.o CanvasElement.o CLNodeFactory.o			

FRAMEWORK_DEPS = clarity.js Util.js clarity.html Makefile

%.o : %.cpp $(FRAMEWORK_DEPS)
	$(ENV) $(CC) $< -o $@ -c $(CFLAGS)

speedtest: speedtest.o ClarityNode.o
	$(CC) -lembind speedtest.o ClarityNode.o $(CFLAGS) -o $(JSOUT)

testbed: testbed.o ClarityNode.o CanvasElement.o
	$(CC) -lembind testbed.o ClarityNode.o CanvasElement.o $(CFLAGS) -o $(JSOUT)

showcase: showcase.o ClarityNode.o CanvasElement.o
	$(CC) -lembind showcase.o ClarityNode.o CanvasElement.o $(CFLAGS) -o $(JSOUT)

docs: clarity.doxyconfig
	doxygen clarity.doxyconfig

clean:
	rm -f *.o 

realclean: clean
	rm -f *.wasm *.wasm.map *.a $(JSOUT)

all: testbed speedtest docs