#pragma once

#include "Vec.hpp"

#include <cmath>

namespace Slic3r {
namespace PluginGeo {

class Point : public Vec2crd
{
public:
    Point() : Vec2crd(0, 0) {}
    Point(coord_t x, coord_t y) : Vec2crd(x, y) {}
    Point(int32_t x, int32_t y) : Vec2crd(coord_t(x), coord_t(y)) {}
    Point(double x, double y) : Vec2crd(coord_t(std::round(x)), coord_t(std::round(y))) {}
    explicit Point(const Vec2d &rhs) : Vec2crd(coord_t(std::round(rhs.x())), coord_t(std::round(rhs.y()))) {}
    template<typename Derived>
    Point(const Eigen::MatrixBase<Derived> &rhs) : Vec2crd(rhs.template cast<coord_t>()) {}

    static Point new_scale(coordf_t x, coordf_t y)
    {
        return Point(coord_t(plugin_geo_scale(x)), coord_t(plugin_geo_scale(y)));
    }
    template<typename Derived>
    static Point new_scale(const Eigen::MatrixBase<Derived> &v)
    {
        return Point(coord_t(plugin_geo_scale(v.x())), coord_t(plugin_geo_scale(v.y())));
    }
};

} // namespace PluginGeo
} // namespace Slic3r
