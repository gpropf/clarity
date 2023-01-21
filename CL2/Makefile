AR 		= emar
CC		= emcc

#ENV		= DEMANGLE_SUPPORT=1 EMCC_DEBUG=1
ENV		= DEMANGLE_SUPPORT=1

#CFLAGS	=  -O0 -g -gsource-map --source-map-base=http://localhost:6931/ -std=c++17
DEBUG_CFLAGS = -O0 -g -std=c++17 -D USETF=1
CFLAGS	= $(DEBUG_CFLAGS)

#CFLAGS	=  -O3 -std=c++17


%.o : %.cpp
	$(ENV) $(CC) $(CFLAGS) $< -o $@ -c


cl2: WebElementBuilder.hpp Signal.hpp
	$(ENV) $(AR) -ars cl2.a WebElementBuilder.o Signal.o

# docs: clarity.doxyconfig
# 	doxygen clarity.doxyconfig

# docsclean:
# 	rm -rf html latex

# clean:
# 	rm -f *.o clarity_embind.js

# realclean: clean docsclean
# 	rm -f *.wasm *.wasm.map *.a $(JSOUT)

# all: showcase speedtest docs