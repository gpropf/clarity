
CC		= em++
CFLAGS	= -s TOTAL_MEMORY=1900mb --bind
JSOUT	= clarity_embind.js
CPPIN	= clarity.cpp

clarity: clarity.cpp clarity.html
	$(CC) $(CFLAGS) -o $(JSOUT) $(CPPIN)


all: clarity