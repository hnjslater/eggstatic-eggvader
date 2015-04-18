#pragma once

class screen_t {
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    int m_width;
    int m_height;
public:
    screen_t() : m_window(NULL), m_renderer(NULL), m_width(0), m_height(0) {
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        throw std::runtime_error( std::string("SDL could not initialize! SDL Error: ") + SDL_GetError() );
    }

        SDL_DisplayMode displayMode;
        SDL_GetCurrentDisplayMode(0, &displayMode);
        m_width = displayMode.w;
        m_height = displayMode.h;
        //Create window
        m_window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                    0, 0, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_INPUT_GRABBED);

        if( m_window == NULL )
        {
                throw std::runtime_error(std::string( "Window could not be created! SDL Error: ") + SDL_GetError() );
        }

        m_renderer = SDL_CreateRenderer( m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
        if( m_renderer == NULL )
        {
                throw std::runtime_error( std::string("Renderer could not be created! SDL Error: ") + SDL_GetError() );
        }

        int imgFlags = IMG_INIT_PNG;
        if( !( IMG_Init( imgFlags ) & imgFlags ) )
        {
                throw std::runtime_error( std::string("SDL_image could not initialize! SDL_image Error: ") + IMG_GetError() );
        }
    }
    screen_t(const screen_t&) = delete;
    screen_t& operator=(const screen_t&) = delete;
    std::shared_ptr<texture_t> make_texture(const std::string& path) {
        return std::make_shared<texture_t>(m_renderer, path);
    }
    void clear() {
        SDL_SetRenderDrawColor( m_renderer, 0x00, 0x00, 0x00, 0x00 );
        SDL_RenderClear( m_renderer );
    }
    void update() {
        SDL_RenderPresent( m_renderer );
    }
    int width() {
        return m_width;
    }
    int height() {
        return m_height;
    }
    ~screen_t() {
    //Destroy window
    SDL_DestroyWindow ( m_window );
        SDL_DestroyRenderer (m_renderer);

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();

    }
};

