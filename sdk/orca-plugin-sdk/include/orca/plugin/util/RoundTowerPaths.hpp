#pragma once

#include "../geometry/Vec.hpp"

#include <cmath>
#include <functional>
#include <vector>

namespace Slic3r {
namespace PluginGeo {
namespace RoundTowerPaths {

inline bool circle_chord_at_y(float y, float cx, float cy, float radius, float x_min, float x_max, float min_len,
                              float &x_low, float &x_high)
{
    const float dy = y - cy;
    if (dy * dy > radius * radius)
        return false;
    const float half = std::sqrt(std::max(0.f, radius * radius - dy * dy));
    x_low            = std::max(x_min, cx - half);
    x_high           = std::min(x_max, cx + half);
    return x_high - x_low > min_len;
}

inline bool circle_chord_at_x(float x, float cx, float cy, float radius, float y_min, float y_max, float min_len,
                              float &y_low, float &y_high)
{
    const float dx = x - cx;
    if (dx * dx > radius * radius)
        return false;
    const float half = std::sqrt(std::max(0.f, radius * radius - dx * dx));
    y_low            = std::max(y_min, cy - half);
    y_high           = std::min(y_max, cy + half);
    return y_high - y_low > min_len;
}

inline float inner_radius_from_box(const Vec2f &box_ld, const Vec2f &box_ru, float perimeter_width)
{
    const float width  = box_ru.x() - box_ld.x();
    const float height = box_ru.y() - box_ld.y();
    return 0.5f * std::min(width, height) - perimeter_width;
}

inline void emit_concentric_rings(const Vec2f &center, float outer_radius, float inner_radius, float ring_step,
                                   const Vec2f &box_ld, const Vec2f &box_ru, float min_chord_len,
                                   const std::function<void(float x1, float y, float x2)> &emit_chord)
{
    for (float r = outer_radius; r >= inner_radius; r -= ring_step) {
        const float y_min = box_ld.y();
        const float y_max = box_ru.y();
        const float dy    = y_max - y_min;
        const int   lines = std::max(1, int(dy / ring_step));
        const float step  = lines > 1 ? dy / (lines - 1) : 0.f;
        float       y     = y_min;
        for (int i = 0; i < lines; ++i) {
            float x_low = 0.f;
            float x_high = 0.f;
            if (circle_chord_at_y(y, center.x(), center.y(), r, box_ld.x(), box_ru.x(), min_chord_len, x_low, x_high))
                emit_chord(x_low, y, x_high);
            if (lines > 1)
                y += step;
        }
    }
}

inline void emit_archimedean_spiral(const Vec2f &center, float inner_radius, float outer_radius, float pitch,
                                    float y_start, float y_end, float min_chord_len,
                                    const std::function<void(float x, float y, float feedrate)> &emit_point,
                                    float feedrate)
{
    if (pitch <= 0.f || y_end <= y_start)
        return;

    const float two_pi = float(2.0 * M_PI);
    float       theta  = 0.f;
    float       r      = inner_radius;
    float       y      = y_start;
    bool        first  = true;

    while (y <= y_end && r <= outer_radius) {
        const float x = center.x() + r * std::cos(theta);
        const float py = center.y() + r * std::sin(theta);
        if (py >= y_start - min_chord_len && py <= y_end + min_chord_len) {
            if (first) {
                emit_point(x, py, feedrate);
                first = false;
            } else {
                emit_point(x, py, feedrate);
            }
        }
        theta += 0.25f;
        r = inner_radius + pitch * theta / two_pi;
        y = py;
    }
}

} // namespace RoundTowerPaths
} // namespace PluginGeo
} // namespace Slic3r
