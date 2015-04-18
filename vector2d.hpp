#pragma once

#include <queue>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <deque>


template<typename T>
class vector2d_t : public std::vector<T> {
    const size_t m_width;
    const size_t m_height;
public:
    vector2d_t(size_t width, size_t height) : std::vector<T>(height*width), m_width(width), m_height(height)  {
        std::vector<T>::resize(width*height);
    }
    T& get(size_t x, size_t y) {
        return (*this)[index_of(x,y)];
    }
    T& get(const std::pair<int, int>& xy) {
        return get(xy.first, xy.second);
    }
    std::vector<std::pair<size_t,size_t>> get_surroundings(size_t l) {
        bool top_edge = l < width();
        bool bot_edge = l >= (width()*height() - width());
        bool left_edge = (l % width()) == 0;
        bool right_edge = (l+1) % width() == 0;

        std::vector<std::pair<size_t,size_t>> surroundings;
        surroundings.reserve(8);

        //NORTH
        if (!top_edge) {
            surroundings.emplace_back(l - width(), 2);
        }
        // SOUTH
        if (!bot_edge) {
            surroundings.emplace_back(l + width(), 2);
        }
        // EAST
        if (!left_edge) {
            surroundings.emplace_back(l-1, 2);
        }
        // WEST
        if (!right_edge) {
            surroundings.emplace_back(l+1, 2);
        }

        //NORTH-EAST
        if (!top_edge && !left_edge) {
            surroundings.emplace_back(l - width() - 1, 3);
        }
        //NORTH-WEST
        if (!top_edge && !right_edge) {
            surroundings.emplace_back(l - width() + 1, 3);
        }
        //SOUTH-EAST
        if (!bot_edge && !left_edge) {
            surroundings.emplace_back(l + width() - 1, 3);
        }
        //SOUTH-WEST
        if (!bot_edge && !right_edge) {
            surroundings.emplace_back(l + width() + 1, 3);
        }
        return surroundings;
    }

    size_t index_of(size_t x, size_t y) {
        return width()*y + x;
    }

    std::pair<size_t, size_t> index_of(size_t i) {
        return std::make_pair(i%width(), i/width());
    }

    size_t width() const {
        return m_width;
    }

    size_t height() const {
        return m_height;
    }
};

template<typename IsGoal, typename IsPassable>
std::vector<std::pair<size_t,size_t>> dijkstra(size_t w, size_t h, size_t x, size_t y, IsGoal is_goal, IsPassable is_passable) {
    vector2d_t<size_t> dvect(w,h);
    for (auto& a : dvect) {
        a = std::numeric_limits<size_t>::max();
    }

    static std::deque<std::pair<size_t,size_t>> todo;
    while (!todo.empty()) {
        todo.pop_front();
    }
    todo.emplace_back(dvect.index_of(x,y),size_t(0));
    dvect[dvect.index_of(x,y)] = 0;
    size_t goal = 0;

    while (!todo.empty()) {
        size_t x;
        size_t v;
        std::tie(x,v) = todo.front();
        todo.pop_front();

        for (auto& s : dvect.get_surroundings(x)) {
            if (dvect[s.first] > v+s.second) {
                size_t cx, cy;
                std::tie(cx, cy) = dvect.index_of(s.first); 
                if (is_goal(cx,cy)) {
                    goal = s.first;
                    goto goal_found;
                }
                if (is_passable(cx, cy)) {
                    dvect[s.first] = v+s.second;
                    todo.emplace_back(s.first,v+s.second);
                }
            }
        }
    }
    // nothing found so stay where we are
    {
        std::vector<std::pair<size_t,size_t>> path;
        path.emplace_back(x,y);
        return path;
    }

    goal_found:
/*
    for (size_t a = 0; a < w; a++) {
        for (size_t b = 0; b < h; b++) {
            if (dvect[dvect.index_of(a,b)] == std::numeric_limits<size_t>::max()) {
                std::cout << "XX" << " ";
            }
            else {
                std::cout << dvect[dvect.index_of(a,b)] << " ";
            }
        }
        std::cout << std::endl;
    }
*/
    std::vector<std::pair<size_t,size_t>> path;
    size_t start = dvect.index_of(x,y);
    size_t current = goal;

    do {
        path.push_back(dvect.index_of(current));
        size_t best_val = std::numeric_limits<size_t>::max();
        for (auto& s : dvect.get_surroundings(current)) {
            if (dvect[s.first] < best_val) {
                current = s.first;
                best_val = dvect[s.first];
            }
        }
    } while (current != start);

    std::reverse(path.begin(), path.end());

    return path;
}

