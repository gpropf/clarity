AR 		= emar
CC		= emcc

#ENV		= DEMANGLE_SUPPORT=1 EMCC_DEBUG=1
ENV		= DEMANGLE_SUPPORT=1

#CFLAGS	=  -O0 -g -gsource-map --source-map-base=http://localhost:6931/ -std=c++17
DEBUG_CFLAGS = -O0 -g -std=c++17 -D USETF=1
CFLAGS	= $(DEBUG_CFLAGS)

#CFLAGS	=  -O3 -std=c++17

JSOUT	= clarity_embind.js

FRAMEWORK_DEPS = JSProxyNode.js Util.js Selectables.js showcase.html Makefile
CLARITY_OBJS = ClarityNode.o CanvasElement.o Selectables.o clarity.o

JSCORE_COMPILE_COMMANDS = --post-js JSProxyNode.js --post-js Util.js --post-js Selectables.js 

%.o : %.cpp $(FRAMEWORK_DEPS)
	$(ENV) $(CC) $(CFLAGS) $< -o $@ -c


# Still have that weird problem with the acorn-optimizer error if you use anything but -O0 or -O1
speedtest-prod: CFLAGS = -O1 -std=c++17 #-sWASM_WORKERS
#speedtest-prod: ENV	= EMCC_DEBUG=2
speedtest-prod: ENV	=
speedtest-prod: speedtest.o ClarityNode.o Selectables.o clarity.o CanvasElement.o
	$(ENV) $(CC) $(CFLAGS) -lembind --pre-js js.d/speedtest.js speedtest.o ClarityNode.o Selectables.o CanvasElement.o clarity.o -o $(JSOUT)


speedtest: speedtest.o ClarityNode.o Selectables.o clarity.o CanvasElement.o
	$(ENV) $(CC) $(CFLAGS) -lembind --pre-js js.d/speedtest.js speedtest.o ClarityNode.o Selectables.o CanvasElement.o clarity.o -o $(JSOUT)


SHOWCASE_OBJS = showcase.o ClarityNode.o CanvasElement.o Selectables.o clarity.o
showcase: $(SHOWCASE_OBJS)
	$(ENV) $(CC) $(CFLAGS) -lembind $(SHOWCASE_OBJS) -o $(JSOUT)

showcase-monolithic: $(SHOWCASE_OBJS)
	$(ENV) $(CC) $(CFLAGS) -lembind $(JSCORE_COMPILE_COMMANDS) $(SHOWCASE_OBJS) -o clout.js

showcase-prod: CFLAGS = -O3 -std=c++17 
showcase-prod: ENV	=
showcase-prod: showcase.o ClarityNode.o CanvasElement.o Selectables.o clarity.o
	$(ENV) $(CC) $(CFLAGS) -lembind showcase.o ClarityNode.o CanvasElement.o Selectables.o clarity.o $(CFLAGS) -o $(JSOUT)

simpletest: simpletest.o ClarityNode.o CanvasElement.o Selectables.o clarity.o
	$(ENV) $(CC) $(CFLAGS) -lembind simpletest.o ClarityNode.o CanvasElement.o Selectables.o clarity.o -o $(JSOUT)

avtest: avtest.o ClarityNode.o CanvasElement.o Selectables.o clarity.o
	$(ENV) $(CC) $(CFLAGS) -lembind --pre-js ActiveVector.js avtest.o ClarityNode.o CanvasElement.o Selectables.o clarity.o -o $(JSOUT)

simpletest-ww: CFLAGS = $(DEBUG_CFLAGS) -sWASM_WORKERS
simpletest-ww: simpletest.o ClarityNode.o CanvasElement.o Selectables.o clarity.o
	$(ENV) $(CC) $(CFLAGS) -lembind simpletest.o ClarityNode.o CanvasElement.o Selectables.o clarity.o -o $(JSOUT)

simpletest-prod: CFLAGS = -O3 -std=c++17 
simpletest-prod: simpletest.o ClarityNode.o CanvasElement.o Selectables.o clarity.o
	$(ENV) $(CC) $(CFLAGS) -lembind simpletest.o ClarityNode.o CanvasElement.o Selectables.o clarity.o -o $(JSOUT)


unittest: unittest.o ClarityNode.o CanvasElement.o Selectables.o clarity.o
	$(ENV) $(CC) $(CFLAGS) -lembind --pre-js Unittest.js unittest.o ClarityNode.o CanvasElement.o Selectables.o clarity.o -o $(JSOUT)

docs: clarity.doxyconfig
	doxygen clarity.doxyconfig

docsclean:
	rm -rf html latex

clean:
	rm -f *.o clarity_embind.js

realclean: clean docsclean
	rm -f *.wasm *.wasm.map *.a $(JSOUT)

all: showcase speedtest docs