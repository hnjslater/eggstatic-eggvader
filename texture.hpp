
class texture_t {
    SDL_Texture* m_texture;
    SDL_Renderer* m_renderer;
    int m_width;
    int m_height;
public:
    texture_t(SDL_Renderer* renderer, const std::string& path) : m_texture(NULL), m_renderer(NULL), m_width(0), m_height(0) {
        SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
        if( loadedSurface == NULL )
        {
            throw std::runtime_error(std::string("Unable to load image ") + path + "! SDL_image Error: " +  IMG_GetError() );
        }
        m_width = loadedSurface->w;
        m_height = loadedSurface->h;
        m_texture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
        if (m_texture == NULL) {
            throw std::runtime_error("balls up");
        }
        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
        m_renderer = renderer;
    }
    texture_t(const texture_t& other) = delete;
    texture_t& operator=(const texture_t& other) = delete;
    
    ~texture_t() {
        //SDL_DestroyTexture( m_texture );
    }
    int width() {
        return m_width;
    }
    int height() {
        return m_height;
    }
    void paint(int x, int y) {
        SDL_Rect drect;
        drect.x = x;
        drect.y = y;
        drect.w = m_width;
        drect.h = m_height;
        
        SDL_RenderCopy(m_renderer, m_texture, NULL, &drect); 
    }
};
