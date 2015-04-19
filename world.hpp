#include <vector2d.hpp>
#include <things.hpp>


class world_t {
    vector2d_t<std::shared_ptr<thing_t>> m_grid;
    

public:
    world_t() : m_grid(1024,768) {
        m_grid.get(0,5) = std::make_shared<egg_t>(0);
        m_grid.get(1,1) = std::make_shared<killer_t>(0);
        m_grid.get(2,1) = std::make_shared<killer_t>(0);
        m_grid.get(1,2) = std::make_shared<killer_t>(0);
        m_grid.get(2,2) = std::make_shared<killer_t>(0);
        for (size_t x = 5; x < 30; x++) {
            for (size_t y = 5; y < 30; y++) {
                m_grid.get(x,y) = std::make_shared<food_t>();

            }
        }
        m_grid.get(32,5) = std::make_shared<egg_t>(1);
    }

    void paint() {
        for (auto x = 0u; x  < m_grid.width(); x++)
            for (auto y = 0u; y < m_grid.height(); y++) {
                auto current = m_grid.get(x,y);
                if (current) {
                    current->paint(x,y);
                }
            }
    }
    void tick() {
        std::vector<std::tuple<size_t, size_t, size_t, size_t>> changes;
        size_t x1, y1, x2, y2;
        bool alive;
        new_things_t new_new_things;
        std::vector<std::tuple<size_t, size_t, std::shared_ptr<thing_t>>> new_things;
        tickargs_t args(m_grid);
        for (x1 = 0; x1 < m_grid.width(); x1++)
            for (y1 = 0; y1 < m_grid.height(); y1++) {
                auto& current = m_grid.get(x1,y1);
                if (current) {
                    args.x = x1;
                    args.y = y1;
                    std::tie(alive,x2,y2,new_new_things) = current->tick(args);
                    for (auto& item : new_new_things) {
                        new_things.emplace_back(x1,y1,item);
                    }
                    if (alive) {
                        if (x1 != x2 || y1 != y2) {
                            changes.emplace_back(x1,y1,x2,y2);
                        }
                    }
                    else {
                        current.reset();
                    }
                }
        }
        
        std::shuffle(changes.begin(), changes.end(), globals::twister);

        for (auto& addition : new_things) {
            std::shared_ptr<thing_t> thing;
            std::tie(x1,y1,thing) = addition;
            for (auto& option : m_grid.get_surroundings(x1,y1)) {
                auto& place = m_grid[std::get<0>(option)];
                if (!place) {
                    place = thing;
                    break;
                }
                
            }
        }
        for (auto change : changes) {
            std::tie(x1,y1,x2,y2) = change;
            auto& next = m_grid.get(x2,y2);
            auto& current = m_grid.get(x1,y1);
            if (!next && current) {
                std::swap(next,current);
            }
        }
    }
    void toggle_select(size_t x, size_t y) {
        auto thing = std::dynamic_pointer_cast<collector_t>(m_grid.get(x, y));
        if (thing && thing->team() == 0) {
            thing->m_selected = !thing->m_selected;
        }
        else {
            for (auto&& item : m_grid) {
                if (item)
                    item->m_selected = false;
            }
        }
    }
    void set_selected_goal(size_t x, size_t y) {

        for (auto item : m_grid) {
            auto itemc = std::dynamic_pointer_cast<collector_t>(item);
            if (itemc && itemc->m_selected) {
                itemc->m_goal = std::make_tuple(true,x,y);
            }
        }
    }

};
