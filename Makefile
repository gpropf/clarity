
ENV		= DEMANGLE_SUPPORT=1 EMCC_DEBUG=1
#EMCC_DEBUG=1 TOTAL_MEMORY=1900mb
# -gsource-map --source-map-base .
# --source-map-base=http://127.0.0.1
CC		= emcc
CFLAGS	=  -lembind -std=c++17 -g3 -gsource-map --source-map-base=file:///home/greg/repos/mgrid
# --source-map-base smap_ -gseparate-dwarf
JSOUT	= clarity_embind.js
CPPIN	= nuke.cpp


clarity: clarity.cpp clarity.html clarity.hpp
	$(ENV) $(CC) $(CPPIN) $(CFLAGS) -o $(JSOUT)


docs: clarity.doxyconfig
	doxygen clarity.doxyconfig


clean:
	rm *.o


all: clarity docs