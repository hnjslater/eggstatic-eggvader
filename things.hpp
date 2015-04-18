#pragma once

class thing_t;


struct tickargs_t {
    tickargs_t(vector2d_t<std::shared_ptr<thing_t>>& grid_in) : x(), y(), grid(grid_in) { }
    size_t x;
    size_t y;
    vector2d_t<std::shared_ptr<thing_t>>& grid;    
};

class thing_t {
public:

    static std::shared_ptr<texture_t> s_selected;
    bool m_selected;
    thing_t() : m_selected(false) { }
    virtual ~thing_t() { }
    virtual void paint(size_t x, size_t y) {
        if (m_selected) {
            thing_t::s_selected->paint(x*s_selected->width(),y*s_selected->height());
        }
    }
    virtual std::pair<size_t,size_t> tick(const tickargs_t& args ) {
        return std::make_pair(args.x,args.y);
    }

};
std::shared_ptr<texture_t> thing_t::s_selected;


class egg_t : public thing_t {
public:
    static std::shared_ptr<texture_t> s_texture;
    virtual ~egg_t() { }
    virtual void paint(size_t x, size_t y) override {
        thing_t::paint(x,y);
        s_texture->paint(x*s_texture->width(),y*s_texture->height());
    }
};
std::shared_ptr<texture_t> egg_t::s_texture;

class collector_t : public thing_t {
public:
    static std::shared_ptr<texture_t> s_texture;
    virtual ~collector_t() { }
    virtual void paint(size_t x, size_t y) override {
        thing_t::paint(x,y);
        s_texture->paint(x*s_texture->width(),y*s_texture->height());
    }
    virtual std::pair<size_t,size_t> tick(const tickargs_t& args) override {

        auto is_passable = [&](size_t x, size_t y) {
            return !static_cast<bool>(args.grid.get(x,y));
        };

        auto is_goal = [](size_t x, size_t y) {
            return (x==1) && (y==1);
        };
       
        auto path = dijkstra(args.grid.width(), args.grid.height(), args.x, args.y, is_goal, is_passable);
        return path[0];
    }
    
};
std::shared_ptr<texture_t> collector_t::s_texture;
