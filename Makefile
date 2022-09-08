
ENV		= DEMANGLE_SUPPORT=1 EMCC_DEBUG=1 TOTAL_MEMORY=1900mb

CC		= emcc

#CFLAGS	=  -O0 -g -gsource-map --source-map-base=http://localhost:6931/ -std=c++17
CFLAGS	=  -O0 -g -std=c++17
#CFLAGS	=  -O3 -std=c++17

JSOUT	= clarity_embind.js


LIBCLR_IN = ControlNetworkNode.cpp ModelNode.hpp CanvasElement.cpp CLNodeFactory.hpp			
LIBCLR_OBJS = ControlNetworkNode.o ModelNode.o CanvasElement.o CLNodeFactory.o			
AR 		= emar
FRAMEWORK_DEPS = clarity.js clarity.html Makefile

# %.cpp : %.hpp

%.o : %.cpp %.hpp $(FRAMEWORK_DEPS)
	$(ENV) $(CC) $< -o $@ -c $(CFLAGS)

testbed: testbed.o ControlNetworkNode.o
	$(CC) -lembind testbed.o ControlNetworkNode.o $(CFLAGS) -o $(JSOUT)

libclr.a: $(LIBCLR_OBJS)
	$(AR) rcs libclr.a $(LIBCLR_OBJS)
	
nuke: nuke.o libclr.a
	$(CC) -lembind nuke.o libclr.a $(CFLAGS) -o $(JSOUT)

docs: clarity.doxyconfig
	doxygen clarity.doxyconfig

clean:
	rm -f *.o 

realclean: clean
	rm -f *.wasm *.wasm.map *.a $(JSOUT)

all: clarity docs simpleTest nuke