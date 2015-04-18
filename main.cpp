#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <memory>
#include <unistd.h>
#include <chrono>
#include <thread>

#include <texture.hpp>
#include <screen.hpp>

#ifdef _WIN32
#include <windows.h>
#endif



int main()
{
    bool ticking = true;
    size_t frame_count = 0;
    size_t too_long = 0;

    using frame_duration = std::chrono::duration<int, std::ratio<1, 45>>;
    screen_t screen;

    auto texture = screen.make_texture("egg.png" );
    /*
    auto wall = screen.make_texture("wall.png" );
    auto food = screen.make_texture("food.png" );
    auto base = screen.make_texture("base.png" );
    auto carry = screen.make_texture("collector_carrying.png" ); */
    bool quit =  false;
    SDL_Event e;
    while( !quit )
    {
        auto start_time = std::chrono::steady_clock::now();
        auto end_time = start_time + frame_duration(1);
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
                //User requests quit
                if( e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                        quit = true;
                }
                else if (e.type == SDL_MOUSEBUTTONUP) {
                    /*
                    int x;
                    int y;
                    SDL_GetMouseState(&x, &y);
                    world.get(x/texture->width(),y/texture->height()) = place_t(place_type_t::FOOD);
                }
                else if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        quit = true;
                    }
                    else {
                        //ticking = !ticking;
                    }*/
                }
        }
        if (ticking) {
            //world.tick();
            //c = 0;
        }

        screen.clear();

        texture->paint(0,0);


        screen.update();
        frame_count++;

    if (end_time < std::chrono::steady_clock::now()) {
        too_long++;
    }
    std::this_thread::sleep_until(end_time);
    if (frame_count == 1000) {
        //quit = true;
    }

    }

    std::cerr << ((double)too_long)/ frame_count << std::endl;
}

#ifdef __WIN32
int CALLBACK WinMain(
    HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
            LPSTR       lpCmdLine,
                int         nCmdShow
                    ) {
    return main();
}
#endif
