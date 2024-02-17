AR 		= emar
CXX		= emcc
ENV		= DEMANGLE_SUPPORT=1

DEBUG_CXXFLAGS = -O0 -g -std=c++17  -I../../
CXXFLAGS	= $(DEBUG_CXXFLAGS)

#-sNO_DISABLE_EXCEPTION_CATCHING

#CXXFLAGS	=  -O3 -std=c++17 -I../../


SRC = $(wildcard *.cpp)

%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# %.o : %.cpp
# 	$(ENV) $(CXX) $(CXXFLAGS) $< -o $@ -c

# --preload-file asset_dir


pixelreactor.js: pixelreactor.o
	$(ENV) $(CXX) $(CXXFLAGS) -lembind pixelreactor.o -o pixelreactor.js -sALLOW_MEMORY_GROWTH


#pixelreactor.html:

#pixelreactor.o: Pixelreactor.hpp
#$(CXX) $(CXXFLAGS) -c -o $@ $<


clean:	
	rm -f *.o *.wasm *.wasm.map *.a pixelreactor.js


-include $(SRC:%.cpp=%.d)

