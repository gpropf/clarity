
ENV		= DEMANGLE_SUPPORT=1 
#EMCC_DEBUG=1 TOTAL_MEMORY=1900mb
# -gsource-map --source-map-base .
# --source-map-base=http://127.0.0.1
CC		= em++
CFLAGS	=  -lembind -std=c++17 -g3 -gsource-map -gseparate-dwarf
# --source-map-base smap_
JSOUT	= clarity_embind.js
CPPIN	= models.cpp


clarity: clarity.cpp clarity.html clarity.hpp
	$(ENV) $(CC) $(CPPIN) $(CFLAGS) -o $(JSOUT)


docs: clarity.doxyconfig
	doxygen clarity.doxyconfig


clean:
	rm *.o


all: clarity docs