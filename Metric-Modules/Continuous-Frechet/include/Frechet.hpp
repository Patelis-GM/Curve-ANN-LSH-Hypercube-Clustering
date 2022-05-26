#pragma once

#include "types.hpp"
#include "interval.hpp"
#include "../../../Curve-Modules/Curve/Curve.h"

namespace Frechet {

    namespace Continuous {

        extern distance_t error;

        struct Distance {
            distance_t value;
            double time_searches;
            double time_bounds;
            std::size_t number_searches;
            std::string repr() const;
        };

        Distance distance(const Curve &, const Curve &);
        Distance _distance(const Curve &, const Curve &, distance_t, distance_t);
        bool _less_than_or_equal(const distance_t, const Curve &, const Curve &, std::vector<Parameters> &, std::vector<Parameters> &, std::vector<Intervals> &, std::vector<Intervals> &);
        distance_t _greedy_upper_bound(const Curve &, const Curve &);
        distance_t _projective_lower_bound(const Curve &, const Curve &);
    }
}
