
ENV		= DEMANGLE_SUPPORT=1 
#EMCC_DEBUG=1

CC		= em++
CFLAGS	= -s TOTAL_MEMORY=1900mb --bind -std=c++17 -gsource-map
# --source-map-base smap_
JSOUT	= clarity_embind.js
CPPIN	= clarity.cpp


clarity: clarity.cpp clarity.html
	$(ENV) $(CC) $(CFLAGS) -o $(JSOUT) $(CPPIN)


docs: clarity.doxyconfig
	doxygen clarity.doxyconfig


clean:
	rm *.o


all: clarity docs