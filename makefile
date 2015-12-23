all: multi_json_map.cpp
	$(CXX) -std=c++11 -I. -o multi_json_map.exe multi_json_map.cpp