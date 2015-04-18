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



