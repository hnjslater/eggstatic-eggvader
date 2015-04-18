#pragma once

class thing_t {
public:


    virtual ~thing_t() { }
    virtual void paint(size_t x, size_t y) = 0;
    virtual std::pair<size_t,size_t> tick(size_t x, size_t y) {
        return std::make_pair(x,y);
    }

};


class egg_t : public thing_t {
public:
    static std::shared_ptr<texture_t> s_texture;
    virtual ~egg_t() { }
    virtual void paint(size_t x, size_t y) override {
        s_texture->paint(x*s_texture->width(),y*s_texture->height());
    }
};
std::shared_ptr<texture_t> egg_t::s_texture;

class collector_t : public thing_t {
public:
    static std::shared_ptr<texture_t> s_texture;
    virtual ~collector_t() { }
    virtual void paint(size_t x, size_t y) override {
        s_texture->paint(x*s_texture->width(),y*s_texture->height());
    }
    virtual std::pair<size_t,size_t> tick(size_t x, size_t y) override {
        return std::make_pair(x-1,y);
    }
    
};
std::shared_ptr<texture_t> collector_t::s_texture;
