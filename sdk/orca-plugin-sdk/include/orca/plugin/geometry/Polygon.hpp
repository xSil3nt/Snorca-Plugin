#pragma once

#include "Point.hpp"

#include <cmath>
#include <initializer_list>
#include <vector>

namespace Slic3r {
namespace PluginGeo {

using Points = std::vector<Point>;

class Polygon
{
public:
    Points points;

    Polygon() = default;
    explicit Polygon(const Points &pts) : points(pts) {}
    Polygon(std::initializer_list<Point> pts) : points(pts) {}
    size_t size() const { return points.size(); }
    bool   empty() const { return points.empty(); }
    bool   is_valid() const { return points.size() >= 3; }

    static Polygon new_scale_circle(const Vec2f &center, float radius, int segments = 48);
};

inline Polygon Polygon::new_scale_circle(const Vec2f &center, float radius, int segments)
{
    Polygon circle;
    circle.points.reserve(size_t(segments));
    for (int i = 0; i < segments; ++i) {
        const float angle = float(2.0 * M_PI * i / segments);
        circle.points.emplace_back(Point::new_scale(coordf_t(center.x() + radius * std::cos(angle)),
                                                    coordf_t(center.y() + radius * std::sin(angle))));
    }
    return circle;
}

} // namespace PluginGeo
} // namespace Slic3r
