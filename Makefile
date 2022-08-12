
ENV		= DEMANGLE_SUPPORT=1 EMCC_DEBUG=1 TOTAL_MEMORY=1900mb
#EMCC_DEBUG=1 TOTAL_MEMORY=1900mb
# -gsource-map --source-map-base .
# --source-map-base=http://127.0.0.1
CC		= emcc
CFLAGS	=  -std=c++17 -g3 -gsource-map
# --source-map-base smap_ -gseparate-dwarf
JSOUT	= clarity_embind.js
# CPPIN	= clarity.cpp ButtonElement.cpp WebElemNode.cpp nuke.cpp
LIBCLR_IN = clarity.cpp ButtonElement.cpp WebElemNode.cpp WebAttrNode.cpp WebNode.cpp \
			ModelNode.cpp ControlNetworkNode.cpp CompoundElement.cpp LabelledInput.cpp \
			LabelElement.cpp CanvasElement.cpp
LIBCLR_OBJS = clarity.o ButtonElement.o WebElemNode.o WebAttrNode.o WebNode.o \
 			ModelNode.o ControlNetworkNode.o CompoundElement.o LabelledInput.o \
			LabelElement.o CanvasElement.o
AR 		= emar
FRAMEWORK_DEPS = clarity.js clarity.html Makefile

# %.cpp : %.hpp

%.o : %.cpp %.hpp $(FRAMEWORK_DEPS)
	$(ENV) $(CC) $< -o $@ -c $(CFLAGS)

simpleTest: simpleTest.o libclr.a
	$(CC) -lembind simpleTest.o libclr.a -o $(JSOUT)

libclr.a: $(LIBCLR_OBJS)
	$(AR) rcs libclr.a $(LIBCLR_OBJS)
	
nuke: nuke.o libclr.a
	$(CC) -lembind nuke.o libclr.a -o $(JSOUT)

docs: clarity.doxyconfig
	doxygen clarity.doxyconfig

clean:
	rm -f *.o 

realclean: clean
	rm -f *.wasm *.wasm.map *.a $(JSOUT)

all: clarity docs simpleTest nuke