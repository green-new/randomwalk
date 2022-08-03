#include "walk.hpp"
#include <time.h>
#include <stdlib.h>
#include <cmath>

void def_randwalk(walk& walkRef) {
    float init = 0.0f;
    walkRef.data(0) = init;
    for (unsigned int i = 1; i < walkRef.walkMax; i++) {
        float move = walkRef.data(i - 1) + (float)(rand() % 10) - 4.4f;
        walkRef.data(i) = move;
    }
}

walk::walk(const unsigned int _walkMax, void (*asdf)(walk& walkRef)) : walkMax(_walkMax) {
    walk_algo = asdf;
    generate();
}

walk::walk() : walk(1000, &def_randwalk) { }

void walk::reseed() {
    seed = rand() * time(0);
    srand(seed);
}

void walk::generate() {
    reseed();
    walk_algo(*this);
}

float& walk::data(unsigned int X) {
    return random_walk[X];
}

const float walk::max_y() { 
    float tmp = 0.0f;
    for (unsigned int i = 0; i < walkMax; i++) {
        float& current = data(i);
        if (current > tmp) {
            tmp = current;
        }
    }
    return tmp;
}

const float walk::min_y() {
    float tmp = 0.0f;
    for (unsigned int i = 0; i < walkMax; i++) {
        float& current = data(i);
        if (current < tmp) {
            tmp = current;
        }
    }
    return tmp;
}