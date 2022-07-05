
ENV		= DEMANGLE_SUPPORT=1 EMCC_DEBUG=1
#EMCC_DEBUG=1 TOTAL_MEMORY=1900mb
# -gsource-map --source-map-base .
# --source-map-base=http://127.0.0.1
CC		= emcc
CFLAGS	=  -std=c++17 -g3 -gsource-map -c
# --source-map-base smap_ -gseparate-dwarf
JSOUT	= clarity_embind.js
CPPIN	= clarity.cpp ButtonElement.cpp WebElemNode.cpp nuke.cpp


clarity: clarity.cpp clarity.html clarity.hpp
	rm -f libclr.a
	$(ENV) $(CC) $(CPPIN) $(CFLAGS)
	emar rcs libclr.a clarity.o ButtonElement.o WebElemNode.o
	emcc -lembind nuke.o libclr.a -o $(JSOUT)
	
# -o $(JSOUT)


docs: clarity.doxyconfig
	doxygen clarity.doxyconfig


clean:
	rm *.o *.wasm *.wasm.map *.a


all: clarity docs