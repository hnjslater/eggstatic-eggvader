ASSETS=egg.png collector.png

a.out: *.hpp *.cpp $(ASSETS)
	g++ --std=c++14 -I /usr/include/SDL2/  -I. main.cpp -lSDL2_image -lSDL2 -Wall -Werror -Wextra -Weffc++ -pthread -O4 -ggdb
#	i586-mingw32msvc-g++ main.cpp -m32 -std=c++1y -I. -pthread


%.png: svg/%.svg
	inkscape --export-png $@ -w 64 -h 64 $< 
