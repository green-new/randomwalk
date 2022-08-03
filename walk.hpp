#pragma once

#define WALKLIMIT       0xFFFF

class walk {
private:
    float random_walk[WALKLIMIT];
    long long seed;
    void (*walk_algo)(walk& walkPtr);
    void reseed();
public:
    const unsigned int walkMax;

    walk();
    walk(const unsigned int _walkMax, void (*f)(walk& walkPtr));

    void generate();
    float& data(unsigned int X);
    const float max_y();
    const float min_y();
};

float normal_distrib(const float mean, const float stddev, float x);