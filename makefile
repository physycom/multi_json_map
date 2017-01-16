
all: dirtree src/html_headers.h src/multi_json_map.cpp
	$(CXX) -std=c++11 -Isrc/jsoncons/src -o bin/multi_json_map.exe src/multi_json_map.cpp

dirtree:
	@mkdir -p bin

clean:
	rm -f bin/multi_json_map.exe

cleanall:
	rm -rf bin obj

