
ENV		= DEMANGLE_SUPPORT=1 EMCC_DEBUG=1 TOTAL_MEMORY=1900mb

CC		= emcc

CFLAGS	=  -O0 -g -gsource-map --source-map-base=http://localhost:6931/ -std=c++17
#CFLAGS	=  -O3 -std=c++17

JSOUT	= clarity_embind.js


LIBCLR_IN = WebElemNode.cpp WebAttrNode.cpp \
			ControlNetworkNode.cpp CompoundElement.cpp LabelledInput.cpp \
			LabelElement.cpp CanvasElement.cpp
LIBCLR_OBJS = WebElemNode.o WebAttrNode.o \
 			ControlNetworkNode.o CompoundElement.o LabelledInput.o \
			LabelElement.o CanvasElement.o
AR 		= emar
FRAMEWORK_DEPS = clarity.js clarity.html Makefile

# %.cpp : %.hpp

%.o : %.cpp %.hpp $(FRAMEWORK_DEPS)
	$(ENV) $(CC) $< -o $@ -c $(CFLAGS)

testbed: testbed.o ControlNetworkNode.o
	$(CC) -lembind testbed.o ControlNetworkNode.o $(CFLAGS) -o $(JSOUT)

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