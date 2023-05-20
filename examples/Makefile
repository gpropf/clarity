AR 		= emar
CC		= emcc

#ENV		= DEMANGLE_SUPPORT=1 EMCC_DEBUG=1
ENV		= DEMANGLE_SUPPORT=1

#CFLAGS	=  -O0 -g -gsource-map --source-map-base=http://localhost:6931/ -std=c++17
CFLAGS_DEBUG = -O0 -g -std=c++17 -I./clarity -D USETF
CFLAGS	=  -O0 -g -std=c++17 -I./clarity -D USETF
#CFLAGS	=  -O3 -std=c++17

JSOUT	= clarity_embind.js

# FRAMEWORK_DEPS = clarity/JSProxyNode.js clarity/Util.js pixelreactor.html Makefile

# %.o : %.cpp $(FRAMEWORK_DEPS)
# 	$(ENV) $(CC) $< -o $@ -c $(CFLAGS)

%.o : %.cpp
	$(ENV) $(CC) $< -o $@ -c $(CFLAGS) 

# Pixelreactor.o: Pixelreactor.hpp
# 	$(ENV) $(CC) pixelreactor.hpp -c $(CFLAGS)

# cd clarity; make ClarityNode.o CanvasElement.o Selectables.o clarity.o

CLARITY_OBJS = clarity/ClarityNode.o clarity/CanvasElement.o clarity/Selectables.o clarity/clarity.o

CLARITY_JS = clarity/JSProxyNode.js clarity/Util.js clarity/Selectables.js
CLARITY_JS_POST_JS = --post-js clarity/JSProxyNode.js --post-js clarity/Util.js --post-js clarity/Selectables.js

PIXELREACTOR_OBJS = pixelreactor.o $(CLARITY_OBJS)
PIXELREACTOR_OUT = pixelreactor.js
MATCHSTICKS_OBJS = matchsticks.o $(CLARITY_OBJS)
MATCHSTICKS_OUT = matchsticks.js

pixelreactor-prod: CFLAGS = -O1 -std=c++17 -I./clarity
pixelreactor-prod: ENV =
pixelreactor-prod: $(PIXELREACTOR_OBJS)	
	$(ENV) $(CC) $(CFLAGS) -lembind --pre-js jsaux/clapps-aux.js $(PIXELREACTOR_OBJS) -o $(JSOUT)

pixelreactor-monolithic: CFLAGS = -O1 -std=c++17 -I./clarity
pixelreactor-monolithic: ENV =
pixelreactor-monolithic: $(PIXELREACTOR_OBJS) $(CLARITY_JS)
	$(ENV) $(CC) $(CFLAGS) -lembind --post-js jsaux/clapps-aux.js --post-js clarity/JSProxyNode.js --post-js clarity/Util.js --post-js clarity/Selectables.js $(PIXELREACTOR_OBJS) -o $(PIXELREACTOR_OUT)
	#cp $(JSOUT) pixelreactor.js

pixelreactor: $(PIXELREACTOR_OBJS)	
	$(ENV) $(CC)  $(CFLAGS) -lembind --pre-js jsaux/clapps-aux.js $(PIXELREACTOR_OBJS) -o $(JSOUT)
	cp $(JSOUT) pixelreactor.js

matchsticks-monolithic: CFLAGS = -O1 -std=c++17 -I./clarity
matchsticks-monolithic: ENV =
matchsticks-monolithic: $(MATCHSTICKS_OBJS) $(CLARITY_JS)
	$(ENV) $(CC) $(CFLAGS) -lembind --post-js jsaux/clapps-aux.js --post-js clarity/JSProxyNode.js --post-js clarity/Util.js --post-js clarity/Selectables.js $(MATCHSTICKS_OBJS) -o $(MATCHSTICKS_OUT)
	#cp $(JSOUT) matchsticks.js


PARAMFACE_OBJS = paramface.o $(CLARITY_OBJS)
PARAMFACE_OUT = paramface.js
paramface: CFLAGS = $(CFLAGS_DEBUG) -I./clarity/CL2
paramface: $(PARAMFACE_OBJS) $(CLARITY_JS)
	$(ENV) $(CC) $(CFLAGS) -lembind --post-js jsaux/clapps-aux.js $(CLARITY_JS_POST_JS) $(PARAMFACE_OBJS) -o $(PARAMFACE_OUT)
	



matchsticks: $(MATCHSTICKS_OBJS)	
	$(ENV) $(CC)  $(CFLAGS) -lembind --pre-js jsaux/clapps-aux.js $(MATCHSTICKS_OBJS) -o $(JSOUT)
	cp $(JSOUT) matchsticks.js

CLDemo.o: CLDemo.hpp
	$(ENV) $(CC) CLDemo.hpp -c $(CFLAGS)
	
cldemo: cldemo.o clarity/ClarityNode.o clarity/CanvasElement.o clarity/Selectables.o clarity/clarity.o
	cd clarity; make ClarityNode.o CanvasElement.o Selectables.o clarity.o
	$(ENV) $(CC) -lembind cldemo.o clarity/ClarityNode.o clarity/CanvasElement.o clarity/Selectables.o clarity/clarity.o $(CFLAGS) -o $(JSOUT)

docs: clarity-apps.doxyconfig
	doxygen clarity-apps.doxyconfig

docsclean:
	rm -rf html latex

clean:
	rm -f *.o matchsticks.js pixelreactor.js

realclean: clean
	cd clarity; make realclean
	rm -f *.wasm *.wasm.map *.a $(JSOUT)

all: pixelreactor matchsticks docs