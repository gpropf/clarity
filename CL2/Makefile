AR 		= emar
CC		= emcc

#ENV		= DEMANGLE_SUPPORT=1 EMCC_DEBUG=1
ENV		= DEMANGLE_SUPPORT=1

#CFLAGS	=  -O0 -g -gsource-map --source-map-base=http://localhost:6931/ -std=c++17
DEBUG_CFLAGS = -O0 -g -std=c++17
CFLAGS	= $(DEBUG_CFLAGS)

#CFLAGS	=  -O3 -std=c++17


%.o : %.cpp
	$(ENV) $(CC) $(CFLAGS) $< -o $@ -c

%.o : %.hpp
	$(ENV) $(CC) $(CFLAGS) $< -o $@ -c

cl2.a: ENV = 
cl2.a: WebElementBuilder.o Signal.o
	rm cl2.a
	$(ENV) $(AR) -crs cl2.a WebElementBuilder.o Signal.o

signalTest: signalTest.o WebElementBuilder.o
	$(ENV) $(CC) $(CFLAGS) -lembind signalTest.o -o signalTest.js
# docs: clarity.doxyconfig
# 	doxygen clarity.doxyconfig

# docsclean:
# 	rm -rf html latex

# clean:
# 	rm -f *.o clarity_embind.js

realclean:
	rm -f *.wasm *.wasm.map *.a signalTest.js

# all: showcase speedtest docs