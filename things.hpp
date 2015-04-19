#pragma once

#include <dijkstra.hpp>

class thing_t;
class collector_t;


struct tickargs_t {
    tickargs_t(vector2d_t<std::shared_ptr<thing_t>>& grid_in) : x(), y(), grid(grid_in) { }
    size_t x;
    size_t y;
    vector2d_t<std::shared_ptr<thing_t>>& grid;    
};

using new_things_t = std::vector<std::shared_ptr<thing_t>>;
using tickrtn_t = std::tuple<bool,size_t,size_t,new_things_t>;

class thing_t {
protected:
    size_t m_team;
    ssize_t m_health;
public:




    thing_t(size_t team) : m_team(team), m_health(50) { }
    thing_t() : m_team(100), m_health(50) { }
    virtual ~thing_t() { }
    virtual void paint(size_t x, size_t y) = 0;

    virtual tickrtn_t tick(const tickargs_t& args ) {
        new_things_t new_things;
        return std::make_tuple(m_health>0,args.x,args.y,new_things);
    }
    size_t team() {
        return m_team;
    }

    void hurt() {
        m_health -= 10;
    }
};


class selectable_thing_t {
public:
    static std::shared_ptr<texture_t> s_selected;
    selectable_thing_t() : m_selected(false) { }
    virtual ~selectable_thing_t() { }
    virtual size_t team() = 0;
    bool m_selected;
    virtual void paint(size_t x, size_t y) {
        if (m_selected) {
            s_selected->paint(x*s_selected->width(),y*s_selected->height());
        }
    }
    virtual void set_goal(size_t x, size_t y) = 0;
};
std::shared_ptr<texture_t> selectable_thing_t::s_selected;

class egg_t : public thing_t {
    size_t m_food;
public:
    static std::array<std::shared_ptr<texture_t>,2> s_textures;
    egg_t(size_t team) : thing_t(team), m_food(100) { }
    virtual ~egg_t() { }
    virtual void paint(size_t x, size_t y) override {
        s_textures[team()]->paint(x*s_textures[0]->width(),y*s_textures[0]->height());
    }
    void give_food(size_t food) {
        m_food += food;
    }
    virtual tickrtn_t tick(const tickargs_t& args ) {
        new_things_t new_things;
        if (m_food > 30) {
            new_things.push_back(std::static_pointer_cast<thing_t>(std::make_shared<collector_t>(team())));
            m_food -= 30;
        }
        return std::make_tuple(m_health>0,args.x,args.y,new_things);
    }

};
std::array<std::shared_ptr<texture_t>,2> egg_t::s_textures;

class food_t : public thing_t {
    size_t m_amount_left;
public:
    static std::array<std::shared_ptr<texture_t>,4> s_textures;

    food_t() : m_amount_left(30) { }
    virtual void paint(size_t x, size_t y) override {
        s_textures[m_amount_left/10]->paint(x*s_textures[0]->width(),y*s_textures[0]->height());
    }
    
    virtual tickrtn_t tick(const tickargs_t& args) override {
        return std::make_tuple(m_amount_left>0,args.x,args.y,new_things_t());
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

class collector_t : public thing_t, public selectable_thing_t {


public:

    static std::array<std::shared_ptr<texture_t>,2> s_textures;
    std::tuple<bool,size_t,size_t> m_goal;
    size_t m_food;


    collector_t(size_t team) : thing_t(team), m_goal(false,0,0), m_food(0) { }
    virtual ~collector_t() { }
    virtual void paint(size_t x, size_t y) override {
        selectable_thing_t::paint(x,y);
        s_textures[m_team]->paint(x*s_textures[0]->width(),y*s_textures[0]->height());
    }
    virtual tickrtn_t tick(const tickargs_t& args) override {
        if (m_health <= 0) {
            return std::make_tuple(false, args.x, args.y, new_things_t());
        }
        if (!std::get<0>(m_goal)) {
            if (m_food < 10) {
                for (auto& place : args.grid.get_surroundings(args.x,args.y)) {
                    auto food_place = std::dynamic_pointer_cast<food_t>(args.grid[place.first]);
                    if (food_place) {
                        food_place->eat();
                        m_food++;
                        return std::make_tuple(true,args.x,args.y,new_things_t());
                    }
                }
            }
            else {
                for (auto& place : args.grid.get_surroundings(args.x,args.y)) {
                    auto egg_place = std::dynamic_pointer_cast<egg_t>(args.grid[place.first]);
                    if (egg_place) {
                        egg_place->give_food(m_food);
                        m_food=0;
                        if (is_food(args.grid.get(std::get<1>(m_goal), std::get<2>(m_goal)))) {
                            m_goal = std::make_tuple(true,std::get<1>(m_goal), std::get<2>(m_goal)); 
                        }
                        return std::make_tuple(true,args.x,args.y,new_things_t());
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

                auto thing = args.grid.get(x,y);
                return (thing && thing->team() == team());
            };
        }

       
        auto path = dijkstra(args.grid.width(), args.grid.height(), args.x, args.y, objective, is_passable);

        if (std::make_tuple(args.x, args.y) != std::make_tuple(std::get<1>(m_goal), std::get<2>(m_goal)) && !is_passable(std::get<0>(path[0]), std::get<1>(path[0]))) {
            m_goal = std::make_tuple(false, std::get<1>(m_goal), std::get<2>(m_goal));

        }
        return std::tuple_cat(std::make_tuple(true), path[0], std::make_tuple(new_things_t()));
    }
    size_t team() {
        return m_team;
    }

    virtual void set_goal(size_t x, size_t y) {
        m_goal = std::make_tuple(true,x,y);
    }
    
};

std::array<std::shared_ptr<texture_t>,2> collector_t::s_textures;


class killer_t : public thing_t, public selectable_thing_t {
public:
    static std::array<std::shared_ptr<texture_t>,2> s_textures;
    size_t m_team;
    std::tuple<bool,size_t,size_t> m_goal;

    killer_t(size_t team) : m_team(team), m_goal() { }
    virtual ~killer_t() { }
    virtual void paint(size_t x, size_t y) override {
        selectable_thing_t::paint(x,y);
        s_textures[m_team]->paint(x*s_textures[0]->width(),y*s_textures[0]->height());
    }
    virtual tickrtn_t tick(const tickargs_t& args) override {

        if (args.x == std::get<1>(m_goal) && args.y == std::get<2>(m_goal)) {
            m_goal = std::make_tuple(false,0,0);
        }
        if (!std::get<0>(m_goal)) {
            for (auto& place : args.grid.get_surroundings(args.x,args.y)) {
                auto thing = args.grid.get(place);
                if (thing && thing->team() != team() && thing->team() < 10) {
                    thing->hurt();
                    return std::make_tuple(true,args.x,args.y,new_things_t());
                }
            }
        }

        auto is_passable = [&](size_t x, size_t y) {
            return !static_cast<bool>(args.grid.get(x,y));
        };
        std::function<bool(size_t,size_t)> objective;
        size_t max_d = std::numeric_limits<size_t>::max();
        if (std::get<0>(m_goal)) {
            objective = [&](size_t x, size_t y) {
                return std::make_tuple(true,x,y) == m_goal;
            };
        }
        else { 
             objective = [&](size_t x, size_t y) {
                auto collector = std::dynamic_pointer_cast<selectable_thing_t>(args.grid.get(x,y));
                return (collector && collector->team() != team());
            };
            max_d = 10;
        }

       
        auto path = dijkstra(args.grid.width(), args.grid.height(), args.x, args.y, max_d, objective, is_passable);

        return std::tuple_cat(std::make_tuple(true), path[0], std::make_tuple(new_things_t()));
    }
    size_t team() {
        return m_team;
    }
    
    virtual void set_goal(size_t x, size_t y) override {
        m_goal = std::make_tuple(true,x,y);
    }
};

std::array<std::shared_ptr<texture_t>,2> killer_t::s_textures;
