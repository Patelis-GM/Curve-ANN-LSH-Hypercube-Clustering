#include <chrono>
#include "Random.h"
#include <random>

/* Utility function to create a random integer in the following range: [from,to] */
int Random::randomInteger(int from, int to) {

    std::default_random_engine defaultRandomEngine(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> distribution(from, to);
    return distribution(defaultRandomEngine);
}

/* Utility function to create a random float in the following range: [from,to) */
float Random::randomFloat(float from, float to) {

    std::default_random_engine defaultRandomEngine(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> distribution(from, to);
    return distribution(defaultRandomEngine);
}

/* Utility function to create a random double in the following range: [from,to) */
double Random::randomDouble(double from, double to) {

    std::default_random_engine defaultRandomEngine(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> distribution(from, to);
    return distribution(defaultRandomEngine);

}

/* Utility function to create a random long double in the following range: [from,to) */
long double Random::randomLongDouble(long double from, long double to) {

    std::default_random_engine defaultRandomEngine(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<long double> distribution(from, to);
    return distribution(defaultRandomEngine);

}