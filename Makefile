
ENV		= DEMANGLE_SUPPORT=1 EMCC_DEBUG=1 TOTAL_MEMORY=1900mb
#EMCC_DEBUG=1 TOTAL_MEMORY=1900mb
# -gsource-map --source-map-base .
# --source-map-base=http://127.0.0.1
CC		= emcc
CFLAGS	=  -std=c++17 -g3 -gsource-map
# --source-map-base smap_ -gseparate-dwarf
JSOUT	= clarity_embind.js
# CPPIN	= clarity.cpp ButtonElement.cpp WebElemNode.cpp nuke.cpp
LIBCLR_IN = clarity.cpp ButtonElement.cpp WebElemNode.cpp WebAttrNode.cpp WebNode.cpp ModelNode.cpp ControlNetworkNode.cpp
LIBCLR_OBJS = clarity.o ButtonElement.o WebElemNode.o WebAttrNode.o WebNode.o \
 			ModelNode.o ControlNetworkNode.o
AR 		= emar
FRAMEWORK_DEPS = clarity.js clarity.html Makefile

# %.cpp : %.hpp

%.o : %.cpp %.hpp $(FRAMEWORK_DEPS)
	$(ENV) $(CC) $< -o $@ -c $(CFLAGS)

libclr.a: $(LIBCLR_OBJS)
	$(AR) rcs libclr.a $(LIBCLR_OBJS)
# clarity: clarity.cpp clarity.html clarity.hpp
# 	rm -f libclr.a
# 	$(ENV) $(CC) $(CPPIN) $(CFLAGS)
# 	# emar rcs libclr.a clarity.o ButtonElement.o WebElemNode.o
# 	emcc -lembind nuke.o libclr.a -o $(JSOUT)
	
nuke: nuke.o libclr.a
	$(CC) -lembind nuke.o libclr.a -o $(JSOUT)

# # -o $(JSOUT)

# libclr.a: $(LIBCLR_IN)
# 	$(ENV) $(CC) $(LIBCLR_IN) $(CFLAGS)

# nuke: libclr.a
# 	emar rcs libclr.a clarity.o ButtonElement.o WebElemNode.o

# docs: clarity.doxyconfig
# 	doxygen clarity.doxyconfig


clean:
	rm -f *.o 

realclean: clean
	rm -f *.wasm *.wasm.map *.a $(JSOUT)

all: clarity docs