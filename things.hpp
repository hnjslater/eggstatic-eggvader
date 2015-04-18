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
    static std::array<std::shared_ptr<texture_t>,3> s_textures;


    food_t() : m_amount_left(30) { }
    virtual void paint(size_t x, size_t y) override {
        thing_t::paint(x,y);
        if (m_amount_left/11 > 0)
        s_textures[m_amount_left/11]->paint(x*s_textures[0]->width(),y*s_textures[0]->height());
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
std::array<std::shared_ptr<texture_t>,3> food_t::s_textures;

bool is_food(const std::shared_ptr<thing_t>& thing) {
    if (!thing) 
        return false;
    return typeid(*thing.get()).hash_code() == typeid(food_t).hash_code();
}

class collector_t : public thing_t {

public:

    static std::shared_ptr<texture_t> s_texture;
    std::pair<size_t,size_t> m_goal;

    collector_t() : m_goal(0,0) { }
    virtual ~collector_t() { }
    virtual void paint(size_t x, size_t y) override {
        thing_t::paint(x,y);
        s_texture->paint(x*s_texture->width(),y*s_texture->height());
    }
    virtual std::tuple<bool,size_t,size_t> tick(const tickargs_t& args) override {
        for (auto& place : args.grid.get_surroundings(args.x,args.y)) {
            auto food_place = std::dynamic_pointer_cast<food_t>(args.grid[place.first]);
            if (food_place) {
                food_place->eat();
                return std::make_tuple(true,args.x,args.y);
            }
        }
        auto is_passable = [&](size_t x, size_t y) {
            return !static_cast<bool>(args.grid.get(x,y));
        };
        auto is_goal = [&](size_t x, size_t y) {
            return is_food(args.grid.get(x,y));
        };
       
        auto path = dijkstra(args.grid.width(), args.grid.height(), args.x, args.y, is_goal, is_passable);
        return std::tuple_cat(std::make_tuple(true), path[0]);
    }
    
};
std::shared_ptr<texture_t> collector_t::s_texture;



