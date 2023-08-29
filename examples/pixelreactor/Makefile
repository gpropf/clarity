AR 		= emar
CXX		= emcc
ENV		= DEMANGLE_SUPPORT=1

DEBUG_CXXFLAGS = -O0 -g -std=c++17 -sALLOW_MEMORY_GROWTH -I../../
#CXXFLAGS	= $(DEBUG_CXXFLAGS) -sNO_DISABLE_EXCEPTION_CATCHING

CXXFLAGS	=  -O3 -std=c++17 -I../../

%.o : %.cpp
	$(ENV) $(CXX) $(CXXFLAGS) $< -o $@ -c




pixelreactor.js: pixelreactor.o
	$(ENV) $(CXX) $(CXXFLAGS) -lembind pixelreactor.o -o pixelreactor.js


pixelreactor.o: Pixelreactor.hpp
#$(CXX) $(CXXFLAGS) -c -o $@ $<


clean:	
	rm -f *.o *.wasm *.wasm.map *.a pixelreactor.js