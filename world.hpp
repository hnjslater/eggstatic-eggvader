#include <vector2d.hpp>
#include <things.hpp>


class world_t {
    vector2d_t<std::shared_ptr<thing_t>> m_grid;
    

public:
    world_t() : m_grid(1024,768) {
        m_grid.get(0,5) = std::make_shared<egg_t>();
        m_grid.get(5,5) = std::make_shared<collector_t>();
    }

    void paint() {
        for (auto x = 0u; x  < m_grid.width(); x++)
            for (auto y = 0u; y < m_grid.height(); y++) {
                auto current = m_grid.get(x,y);
                if (current)
                    current->paint(x,y);
            }
    }
    void tick() {
        std::vector<std::tuple<size_t, size_t, size_t, size_t>> changes;
        size_t x1, y1, x2, y2;
        for (x1 = 0; x1 < m_grid.width(); x1++)
            for (y1 = 0; y1 < m_grid.height(); y1++) {
                auto current = m_grid.get(x1,y1);
                if (current) {
                    std::tie(x2,y2) = current->tick(x1,y1);
                    if (x1 != x2 || y1 != y2) {
                        changes.emplace_back(x1,y1,x2,y2);
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

};
