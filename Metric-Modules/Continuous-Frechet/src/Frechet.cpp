#include <vector>
#include <limits>
#include <ctime>
#include <algorithm>
#include <sstream>
#include "../include/Frechet.hpp"

namespace Frechet {

    namespace Continuous {

        distance_t error = 1;
        bool round = true;

        std::string Distance::repr() const {
            std::stringstream ss;
            ss << value;
            return ss.str();
        }

        Distance distance(const Curve &curve1, const Curve &curve2) {
            if ((curve1.getComplexity() < 2) or (curve2.getComplexity() < 2)) {
                std::cerr << "WARNING: comparison possible only for curves of at least two points" << std::endl;
                Distance result;
                result.value = std::numeric_limits<distance_t>::signaling_NaN();
                return result;
            }
            if (curve1.getDimensions() != curve2.getDimensions()) {
                std::cerr << "WARNING: comparison possible only for curves of equal number of dimensions" << std::endl;
                Distance result;
                result.value = std::numeric_limits<distance_t>::signaling_NaN();
                return result;
            }

            const auto start = std::clock();
            const distance_t lb = _projective_lower_bound(curve1, curve2);
            const distance_t ub = _greedy_upper_bound(curve1, curve2);
            const auto end = std::clock();

            auto dist = _distance(curve1, curve2, ub, lb);
            dist.time_bounds = (end - start) / CLOCKS_PER_SEC;

            return dist;
        }

        Distance _distance(const Curve &curve1, const Curve &curve2, distance_t ub, distance_t lb) {
            Distance result;
            const auto start = std::clock();

            distance_t split = (ub + lb) / 2;
            const distance_t p_error = lb * error / 100 > std::numeric_limits<distance_t>::epsilon() ? lb * error / 100 : std::numeric_limits<distance_t>::epsilon();
            std::size_t number_searches = 0;

            if (ub - lb > p_error) {

                const auto infty = std::numeric_limits<parameter_t>::infinity();
                std::vector<Parameters> reachable1(curve1.getComplexity() - 1, Parameters(curve2.getComplexity(), infty));
                std::vector<Parameters> reachable2(curve1.getComplexity(), Parameters(curve2.getComplexity() - 1, infty));

                std::vector<Intervals> free_intervals1(curve2.getComplexity(), Intervals(curve1.getComplexity(), Interval()));
                std::vector<Intervals> free_intervals2(curve1.getComplexity(), Intervals(curve2.getComplexity(), Interval()));

                if (std::isnan(lb) or std::isnan(ub)) {
                    result.value = std::numeric_limits<distance_t>::signaling_NaN();
                    return result;
                }

                //Binary search over the feasible distances
                while (ub - lb > p_error) {
                    ++number_searches;
                    split = (ub + lb) / distance_t(2);
                    if (split == lb or split == ub) break;
                    auto isLessThan = _less_than_or_equal(split, curve1, curve2, reachable1, reachable2, free_intervals1, free_intervals2);
                    if (isLessThan) {
                        ub = split;
                    }
                    else {
                        lb = split;
                    }
                }
            }

            const auto end = std::clock();
            result.value = lb;
            result.time_searches = (end - start) / CLOCKS_PER_SEC;
            result.number_searches = number_searches;
            return result;
        }

        bool _less_than_or_equal(const distance_t distance, Curve const &curve1, Curve const &curve2, std::vector<Parameters> &reachable1, std::vector<Parameters> &reachable2, std::vector<Intervals> &free_intervals1, std::vector<Intervals> &free_intervals2) {

            const distance_t dist_sqr = distance * distance;
            const auto infty = std::numeric_limits<parameter_t>::infinity();
            const curve_size_t n1 = curve1.getComplexity();
            const curve_size_t n2 = curve2.getComplexity();

#pragma omp parallel for collapse(2) if (n1 * n2 > 1000)
            for (curve_size_t i = 0; i < n1; ++i) {
                for (curve_size_t j = 0; j < n2; ++j) {
                    if (i < n1 - 1) reachable1[i][j] = infty;
                    if (j < n2 - 1) reachable2[i][j] = infty;
                    free_intervals1[j][i].reset();
                    free_intervals2[i][j].reset();
                }
            }

            for (curve_size_t i = 0; i < n1 - 1; ++i) {
                reachable1[i][0] = 0;
                if (curve2[0].dist_sqr(curve1[i + 1]) > dist_sqr) break;
            }

            for (curve_size_t j = 0; j < n2 - 1; ++j) {
                reachable2[0][j] = 0;
                if (curve1[0].dist_sqr(curve2[j + 1]) > dist_sqr) break;
            }

#pragma omp parallel for collapse(2) if (n1 * n2 > 1000)
            for (curve_size_t i = 0; i < n1; ++i) {
                for (curve_size_t j = 0; j < n2; ++j) {
                    if ((i < n1 - 1) and (j > 0)) {
                        free_intervals1[j][i] = curve2[j].ball_intersection_interval(dist_sqr, curve1[i], curve1[i + 1]);
                    }
                    if ((j < n2 - 1) and (i > 0)) {
                        free_intervals2[i][j] = curve1[i].ball_intersection_interval(dist_sqr, curve2[j], curve2[j + 1]);
                    }
                }
            }

            for (curve_size_t i = 0; i < n1; ++i) {
                for (curve_size_t j = 0; j < n2; ++j) {
                    if ((i < n1 - 1) and (j > 0)) {
                        if (not free_intervals1[j][i].empty()) {
                            if (reachable2[i][j - 1] != infty) {
                                reachable1[i][j] = free_intervals1[j][i].begin();
                            }
                            else if (reachable1[i][j - 1] <= free_intervals1[j][i].end()) {
                                reachable1[i][j] = std::max(free_intervals1[j][i].begin(), reachable1[i][j - 1]);
                            }
                        }
                    }
                    if ((j < n2 - 1) and (i > 0)) {
                        if (not free_intervals2[i][j].empty()) {
                            if (reachable1[i - 1][j] != infty) {
                                reachable2[i][j] = free_intervals2[i][j].begin();
                            }
                            else if (reachable2[i - 1][j] <= free_intervals2[i][j].end()) {
                                reachable2[i][j] = std::max(free_intervals2[i][j].begin(), reachable2[i - 1][j]);
                            }
                        }
                    }
                }
            }
            return reachable1.back().back() < infty;
        }

        distance_t _greedy_upper_bound(const Curve &curve1, const Curve &curve2) {
            distance_t result = 0;

            const curve_size_t len1 = curve1.getComplexity(), len2 = curve2.getComplexity();
            curve_size_t i = 0, j = 0;

            while ((i < len1 - 1) and (j < len2 - 1)) {
                result = std::max(result, curve1[i].dist_sqr(curve2[j]));

                distance_t dist1 = curve1[i + 1].dist_sqr(curve2[j]), dist2 = curve1[i].dist_sqr(curve2[j + 1]), dist3 = curve1[i + 1].dist_sqr(curve2[j + 1]);

                if ((dist1 <= dist2) and (dist1 <= dist3)) ++i;
                else if ((dist2 <= dist1) and (dist2 <= dist3)) ++j;
                else {
                    ++i;
                    ++j;
                }
            }

            while (i < len1) result = std::max(result, curve1[i++].dist_sqr(curve2[j]));

            --i;

            while (j < len2) result = std::max(result, curve1[i].dist_sqr(curve2[j++]));

            return std::sqrt(result);
        }

        distance_t _projective_lower_bound(const Curve &curve1, const Curve &curve2) {
            std::vector<distance_t> distances1_sqr = std::vector<distance_t>(curve2.getComplexity() - 1), distances2_sqr = std::vector<distance_t>(curve1.getComplexity() + curve2.getComplexity() + 2);

            for (curve_size_t i = 0; i < curve1.getComplexity(); ++i) {
#pragma omp parallel for
                for (curve_size_t j = 0; j < curve2.getComplexity() - 1; ++j) {
                    if (curve2[j].dist_sqr(curve2[j + 1]) > 0) {
                        distances1_sqr[j] = curve1[i].line_segment_dist_sqr(curve2[j], curve2[j + 1]);
                    }
                    else {
                        distances1_sqr[j] = curve1[i].dist_sqr(curve2[j]);
                    }
                }
                distances2_sqr[i] = *std::min_element(distances1_sqr.begin(), distances1_sqr.end());
            }

            distances1_sqr = std::vector<distance_t>(curve1.getComplexity() - 1);

            for (curve_size_t i = 0; i < curve2.getComplexity(); ++i) {
#pragma omp parallel for
                for (curve_size_t j = 0; j < curve1.getComplexity() - 1; ++j) {
                    if (curve1[j].dist_sqr(curve1[j + 1]) > 0) {
                        distances1_sqr[j] = curve2[i].line_segment_dist_sqr(curve1[j], curve1[j + 1]);
                    }
                    else {
                        distances1_sqr[j] = curve2[i].dist_sqr(curve1[j]);
                    }
                }
                distances2_sqr[curve1.getComplexity() + i] = *std::min_element(distances1_sqr.begin(), distances1_sqr.end());
            }

            distances2_sqr[curve1.getComplexity() + curve2.getComplexity()] = curve1[0].dist_sqr(curve2[0]);
            distances2_sqr[curve1.getComplexity() + curve2.getComplexity() + 1] = curve1[curve1.getComplexity() - 1].dist_sqr(curve2[curve2.getComplexity() - 1]);
            return std::sqrt(*std::max_element(distances2_sqr.begin(), distances2_sqr.end()));
        }

    }
}
