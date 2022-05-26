#pragma once

#include <vector>
#include <cmath>
#include <limits>

typedef double distance_t;
typedef double coordinate_t;
typedef long double parameter_t;

typedef unsigned long dimensions_t;
typedef unsigned long curve_size_t;
typedef unsigned long curve_number_t;

class Point;
class Curve;
class Interval;

using Intervals = std::vector<Interval>;
using Coordinates = std::vector<coordinate_t>;
using Distances = std::vector<distance_t>;
using Curve_Numbers = std::vector<curve_number_t>;
using Parameters = std::vector<parameter_t>;


template<typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
inline bool near_eq(T x, T y) {
    return std::abs(x - y) <= std::min(std::abs(x), std::abs(y)) * std::numeric_limits<T>::epsilon();
}