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
#include <random>
#include <algorithm>
namespace globals {
    std::mt19937 twister;
}

#include <texture.hpp>
#include <screen.hpp>
#include <world.hpp>

#ifdef _WIN32
#include <windows.h>
#endif



int main()
{
    std::random_device rd;
    globals::twister  = std::mt19937(rd());

    bool ticking = true;
    size_t since_last_tick = 0;
    size_t frame_count = 0;
    size_t too_long = 0;
    using frame_duration = std::chrono::duration<int, std::ratio<1, 45>>;
    screen_t screen;
    egg_t::s_textures[0] = screen.make_texture("egg.png" );
    egg_t::s_textures[1] = screen.make_texture("house.png" );
    collector_t::s_textures[0] = screen.make_texture("collector.png" );
    collector_t::s_textures[1] = screen.make_texture("person.png" );
    selectable_thing_t::s_selected = screen.make_texture("selected.png" );
    food_t::s_textures[3] = screen.make_texture("food1.png");
    food_t::s_textures[2] = screen.make_texture("food2.png");
    food_t::s_textures[1] = screen.make_texture("food3.png");
    food_t::s_textures[0] = screen.make_texture("food4.png");
    killer_t::s_textures[0] = screen.make_texture("killer.png");
    bool quit =  false;

    world_t world;

    while( !quit )
    {
        SDL_Event e;
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
                    int x;
                    int y;
                    size_t w = collector_t::s_textures[0]->width();
                    size_t h = collector_t::s_textures[0]->height();
                    SDL_GetMouseState(&x, &y);
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        world.toggle_select(x/w, y/h);
                    }
                    else {
                        world.set_selected_goal(x/w,y/h);
                    }
                }
                /*
                else if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        quit = true;
                    }
                    else {
                        //ticking = !ticking;
                    }*/
        }
        if (ticking && since_last_tick > 10) {
            world.tick();
            since_last_tick = 0;
        }
        else {
            since_last_tick++;
        }


        screen.clear();

        world.paint();


        frame_count++;
        screen.update();

    if (end_time < std::chrono::steady_clock::now()) {
        too_long++;
    }
    std::this_thread::sleep_until(end_time);

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
