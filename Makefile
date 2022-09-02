
ENV		= DEMANGLE_SUPPORT=1 EMCC_DEBUG=1 TOTAL_MEMORY=1900mb
#EMCC_DEBUG=1 TOTAL_MEMORY=1900mb
# -gsource-map --source-map-base .
# --source-map-base=http://127.0.0.1
CC		= emcc
CFLAGS	=  -O0 -g -gsource-map --source-map-base=http://localhost:6931/ -std=c++17
#CFLAGS	=  -std=c++17 -g
# -gsource-map
# --source-map-base smap_ -gseparate-dwarf
JSOUT	= clarity_embind.js
NUKE_HTML_OUT = nuke.html
NUKE_JS_OUT = nuke.js

# CPPIN	= clarity.cpp ButtonElement.cpp WebElemNode.cpp nuke.cpp
LIBCLR_IN = ButtonElement.cpp WebElemNode.cpp WebAttrNode.cpp \
			ControlNetworkNode.cpp CompoundElement.cpp LabelledInput.cpp \
			LabelElement.cpp CanvasElement.cpp CLNodeFactory.cpp
LIBCLR_OBJS = ButtonElement.o WebElemNode.o WebAttrNode.o \
 			ControlNetworkNode.o CompoundElement.o LabelledInput.o \
			LabelElement.o CanvasElement.o CLNodeFactory.o
AR 		= emar
FRAMEWORK_DEPS = clarity.js clarity.html Makefile

# %.cpp : %.hpp

%.o : %.cpp %.hpp $(FRAMEWORK_DEPS)
	$(ENV) $(CC) $< -o $@ -c $(CFLAGS)

testbed: testbed.o libclr.a
	$(CC) -lembind testbed.o libclr.a $(CFLAGS) -o $(JSOUT)

debugtest: debugtest.cpp
	#$(CC) debugtest.cpp -g --source-map-base=http://localhost:6931/ -gsource-map -o debug.html
	$(CC) debugtest.cpp -g -o debug.html


simpleTest: simpleTest.o libclr.a
	$(CC) -lembind simpleTest.o libclr.a -o $(JSOUT)

libclr.a: $(LIBCLR_OBJS)
	$(AR) rcs libclr.a $(LIBCLR_OBJS)
	
nuke: nuke.o libclr.a
	$(CC) -lembind nuke.o libclr.a $(CFLAGS) -o $(JSOUT)

nukehtml: nuke.o libclr.a
	$(CC) -lembind nuke.o libclr.a $(CFLAGS) -o $(NUKE_HTML_OUT)

nukejs: nuke.o libclr.a
	$(CC) -lembind nuke.o libclr.a $(CFLAGS) -o $(NUKE_JS_OUT)

docs: clarity.doxyconfig
	doxygen clarity.doxyconfig

clean:
	rm -f *.o 

realclean: clean
	rm -f *.wasm *.wasm.map *.a $(JSOUT)

all: clarity docs simpleTest nuke