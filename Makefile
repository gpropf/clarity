AR 		= emar
CC		= emcc

#ENV		= DEMANGLE_SUPPORT=1 EMCC_DEBUG=1
ENV		= DEMANGLE_SUPPORT=1

#CFLAGS	=  -O0 -g -gsource-map --source-map-base=http://localhost:6931/ -std=c++17
CFLAGS	=  -O0 -g -std=c++17 -D USETF=1 #-sASSERTIONS
#CFLAGS	=  -O3 -std=c++17

JSOUT	= clarity_embind.js

LIBCLR_IN = ClarityNode.cpp ModelNode.hpp CanvasElement.cpp CLNodeFactory.hpp			
LIBCLR_OBJS = ClarityNode.o ModelNode.o CanvasElement.o CLNodeFactory.o			

FRAMEWORK_DEPS = JSProxyNode.js Util.js showcase.html Makefile

%.o : %.cpp $(FRAMEWORK_DEPS)
	$(ENV) $(CC) $< -o $@ -c $(CFLAGS) 

# speedtest: speedtest.o ClarityNode.o Selectables.o clarity.o
# 	$(CC) -lembind speedtest.o ClarityNode.o Selectables.o clarity.o $(CFLAGS) -o $(JSOUT)

# Still have that weird problem with the acorn-optimizer error if you use anything but -O0 or -O1
speedtest-prod: CFLAGS = -O1 -std=c++17 
#speedtest-prod: ENV	= EMCC_DEBUG=2
#speedtest-prod: ENV	=
speedtest-prod: speedtest.o ClarityNode.o Selectables.o clarity.o
	$(CC) -lembind --pre-js js.d/speedtest.js speedtest.o ClarityNode.o Selectables.o clarity.o $(CFLAGS) -o $(JSOUT)


speedtest: speedtest.o ClarityNode.o Selectables.o clarity.o
	$(CC) -lembind --pre-js js.d/speedtest.js speedtest.o ClarityNode.o Selectables.o clarity.o $(CFLAGS) -o $(JSOUT)

showcase: showcase.o ClarityNode.o CanvasElement.o Selectables.o clarity.o
	$(CC) -lembind showcase.o ClarityNode.o CanvasElement.o Selectables.o clarity.o $(CFLAGS) -o $(JSOUT)

showcase-prod: CFLAGS = -O3 -std=c++17 
showcase-prod: showcase.o ClarityNode.o CanvasElement.o Selectables.o clarity.o
	$(CC) -lembind showcase.o ClarityNode.o CanvasElement.o Selectables.o clarity.o $(CFLAGS) -o $(JSOUT)

simpletest: simpletest.o ClarityNode.o CanvasElement.o Selectables.o clarity.o
	$(CC) -lembind simpletest.o ClarityNode.o CanvasElement.o Selectables.o clarity.o $(CFLAGS) -o $(JSOUT)

simpletest-prod: CFLAGS = -O3 -std=c++17 
simpletest-prod: simpletest.o ClarityNode.o CanvasElement.o Selectables.o clarity.o
	$(CC) -lembind simpletest.o ClarityNode.o CanvasElement.o Selectables.o clarity.o $(CFLAGS) -o $(JSOUT)



unittest: unittest.o ClarityNode.o CanvasElement.o Selectables.o clarity.o
	$(CC) -lembind --pre-js Unittest.js unittest.o ClarityNode.o CanvasElement.o Selectables.o clarity.o $(CFLAGS) -o $(JSOUT)

docs: clarity.doxyconfig
	doxygen clarity.doxyconfig

docsclean:
	rm -rf html latex

clean:
	rm -f *.o clarity_embind.js

realclean: clean docsclean
	rm -f *.wasm *.wasm.map *.a $(JSOUT)

all: showcase speedtest docs