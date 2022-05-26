#pragma once

#include <vector>
#include <iostream>
#include <limits>

#include "types.hpp"

class Interval {
    parameter_t beg, en;

public:
    Interval() : beg{parameter_t(1)}, en{parameter_t(0)} {}

    Interval(const parameter_t begin, const parameter_t end) : beg{begin}, en{end} {}

    inline bool operator<(const Interval &other) const {
        return (beg < other.begin()) or ((beg == other.begin()) and (en < other.end()));
    }

    inline bool empty() const {
        if (en - beg >= std::numeric_limits<parameter_t>::epsilon()) return beg > en;
        else return true;
    }

    inline bool intersects(const Interval &other) const {
        if (empty() or other.empty()) return false;

        return ((other.beg >= beg) and (other.beg <= en)) or ((other.en >= beg) and (other.en <= en)) or ((other.beg <= beg) and (other.en >= en));
    }

    inline parameter_t begin() const {
        return beg;
    }

    inline parameter_t end() const {
        return en;
    }

    inline void reset() {
        beg = parameter_t(1);
        en = parameter_t(0);
    }
};

std::ostream &operator<<(std::ostream &, const Interval &);
