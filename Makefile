
CC		= em++
CFLAGS	= -s TOTAL_MEMORY=1900mb --bind -std=c++17 -gsource-map
JSOUT	= clarity_embind.js
CPPIN	= clarity.cpp


clarity: clarity.cpp clarity.html
	$(CC) $(CFLAGS) -o $(JSOUT) $(CPPIN)


docs: clarity.doxyconfig
	doxygen clarity.doxyconfig


clean:
	rm *.o


all: clarity docs