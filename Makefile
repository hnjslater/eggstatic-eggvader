ASSETS=egg.png collector.png selected.png food1.png food2.png food3.png food4.png house.png person.png killer.png

a.out: *.hpp *.cpp $(ASSETS)
	g++ --std=c++14 -I /usr/include/SDL2/  -I. main.cpp -lSDL2_image -lSDL2 -Wall -Werror -Wextra -Weffc++ -pthread -O4
#	i586-mingw32msvc-g++ main.cpp -m32 -std=c++1y -I. -pthread


%.png: svg/%.svg
	inkscape --export-png $@ -w 32 -h 32 $< 
