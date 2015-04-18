#pragma once

#include <dijkstra.hpp>

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
    virtual std::tuple<bool,size_t,size_t> tick(const tickargs_t& args ) {
        return std::make_tuple(true,args.x,args.y);
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

class food_t : public thing_t {
    size_t m_amount_left;
public:
    static std::array<std::shared_ptr<texture_t>,4> s_textures;


    food_t() : m_amount_left(30) { }
    virtual void paint(size_t x, size_t y) override {
        thing_t::paint(x,y);
        s_textures[m_amount_left/10]->paint(x*s_textures[0]->width(),y*s_textures[0]->height());
    }

    
    virtual std::tuple<bool,size_t,size_t> tick(const tickargs_t& args) override {
        return std::make_tuple(m_amount_left>0,args.x,args.y);
    }

    void eat() {
        if (m_amount_left > 0) {
            m_amount_left--;
        }
    }
};
std::array<std::shared_ptr<texture_t>,4> food_t::s_textures;

template<typename T>
struct is {
bool operator()(const std::shared_ptr<thing_t>& thing) {
    if (!thing) 
        return false;
    return typeid(*thing.get()).hash_code() == typeid(T).hash_code();
}
};

is<food_t> is_food;
is<egg_t> is_egg;

class collector_t : public thing_t {

public:

    static std::shared_ptr<texture_t> s_texture;
    std::tuple<bool,size_t,size_t> m_goal;
    size_t m_food;
    

    collector_t() : m_goal(false,0,0), m_food(0) { }
    virtual ~collector_t() { }
    virtual void paint(size_t x, size_t y) override {
        thing_t::paint(x,y);
        s_texture->paint(x*s_texture->width(),y*s_texture->height());
    }
    virtual std::tuple<bool,size_t,size_t> tick(const tickargs_t& args) override {
        if (!std::get<0>(m_goal)) {
            if (m_food < 10) {
                for (auto& place : args.grid.get_surroundings(args.x,args.y)) {
                    auto food_place = std::dynamic_pointer_cast<food_t>(args.grid[place.first]);
                    if (food_place) {
                        food_place->eat();
                        m_food++;
                        return std::make_tuple(true,args.x,args.y);
                    }
                }
            }
            else {
                for (auto& place : args.grid.get_surroundings(args.x,args.y)) {
                    auto egg_place = std::dynamic_pointer_cast<egg_t>(args.grid[place.first]);
                    if (egg_place) {
                        m_food=0;
                        if (is_food(args.grid.get(std::get<1>(m_goal), std::get<2>(m_goal)))) {
                            m_goal = std::make_tuple(true,std::get<1>(m_goal), std::get<2>(m_goal)); 
                        }
                        return std::make_tuple(true,args.x,args.y);
                    }
                }
                
            }
        }
        auto is_passable = [&](size_t x, size_t y) {
            return !static_cast<bool>(args.grid.get(x,y));
        };
        std::function<bool(size_t,size_t)> objective;
        if (std::get<0>(m_goal)) {
            objective = [&](size_t x, size_t y) {
                return std::make_tuple(true,x,y) == m_goal;
            };
        }
        else if (m_food < 10) {
            objective = [&](size_t x, size_t y) {
                return is_food(args.grid.get(x,y));
            };
        }
        else {
            objective = [&](size_t x, size_t y) {
                return is_egg(args.grid.get(x,y));
            };
        }

       
        auto path = dijkstra(args.grid.width(), args.grid.height(), args.x, args.y, objective, is_passable);

        if (std::make_tuple(args.x, args.y) != std::make_tuple(std::get<1>(m_goal), std::get<2>(m_goal)) && !is_passable(std::get<0>(path[0]), std::get<1>(path[0]))) {
            m_goal = std::make_tuple(false, std::get<1>(m_goal), std::get<2>(m_goal));

        }
        return std::tuple_cat(std::make_tuple(true), path[0]);
    }
    
};
std::shared_ptr<texture_t> collector_t::s_texture;



