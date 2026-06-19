#pragma once

#include "../IWipeTowerPathWriter.hpp"
#include "../geometry/Vec.hpp"

#include <cmath>
#include <functional>

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

inline float inner_radius_from_box(const Vec2f &box_ld, const Vec2f &box_ru, float inset)
{
    const float width  = box_ru.x() - box_ld.x();
    const float height = box_ru.y() - box_ld.y();
    return std::max(0.f, 0.5f * std::min(width, height) - inset);
}

inline float circle_circumference(float radius)
{
    return float(2.0 * M_PI) * radius;
}

// Closed circular perimeter (true concentric ring), not horizontal chord fill.
inline void emit_circle_loop(IWipeTowerPathWriter &writer, const Vec2f &center, float radius, float feedrate,
                             int segments = 48)
{
    if (radius <= 0.f || segments < 3)
        return;

    for (int i = 0; i <= segments; ++i) {
        const float angle = float(2.0 * M_PI * i / segments);
        const float x     = center.x() + radius * std::cos(angle);
        const float y     = center.y() + radius * std::sin(angle);
        if (i == 0)
            writer.travel(x, y, feedrate);
        else
            writer.extrude(x, y, feedrate);
    }
}

inline void emit_concentric_ring_loops(IWipeTowerPathWriter &writer, const Vec2f &center, float outer_radius,
                                       float inner_radius, float ring_step, float feedrate, bool sparse)
{
    if (ring_step <= 0.f)
        return;

    int ring_index = 0;
    for (float r = outer_radius; r >= inner_radius - 1e-4f; r -= ring_step, ++ring_index) {
        if (sparse && (ring_index % 2) != 0)
            continue;
        emit_circle_loop(writer, center, r, feedrate);
    }
}

// Archimedean spiral clipped to a maximum radius; stops when accumulated path length is reached.
inline void emit_archimedean_spiral_wipe(IWipeTowerPathWriter &writer, const Vec2f &center, float max_radius,
                                         float pitch, float feedrate, float length_target)
{
    if (pitch <= 0.f || max_radius <= 0.f || length_target <= 0.f)
        return;

    const float two_pi     = float(2.0 * M_PI);
    const float theta_step = 0.35f;
    float       theta      = 0.f;
    float       accumulated = 0.f;

    auto point_at = [&](float t) {
        const float r = std::min(max_radius, pitch * t / two_pi);
        return Vec2f(center.x() + r * std::cos(t), center.y() + r * std::sin(t));
    };

    Vec2f prev = point_at(theta);
    writer.travel(prev, feedrate);

    while (accumulated < length_target) {
        theta += theta_step;
        const Vec2f pt  = point_at(theta);
        const float dx  = pt.x() - prev.x();
        const float dy  = pt.y() - prev.y();
        const float len = std::sqrt(dx * dx + dy * dy);
        if (len < 1e-5f)
            continue;
        writer.extrude(pt, feedrate);
        accumulated += len;
        prev = pt;
    }
}

// Distribute a fixed extrusion amount along a short spiral segment (ramming).
inline void emit_round_ram_segment(IWipeTowerPathWriter &writer, const Vec2f &center, float max_radius, float pitch,
                                   float &theta, float segment_length, float segment_e, float feedrate)
{
    if (segment_length <= 0.f || segment_e <= 0.f)
        return;

    const float two_pi      = float(2.0 * M_PI);
    const float theta_step  = 0.25f;
    float       accumulated = 0.f;
    float       e_used        = 0.f;

    auto point_at = [&](float t) {
        const float r = std::min(max_radius, pitch * t / two_pi);
        return Vec2f(center.x() + r * std::cos(t), center.y() + r * std::sin(t));
    };

    Vec2f prev = point_at(theta);
    while (accumulated < segment_length - 1e-4f) {
        theta += theta_step;
        const Vec2f pt  = point_at(theta);
        const float len = std::sqrt((pt.x() - prev.x()) * (pt.x() - prev.x()) + (pt.y() - prev.y()) * (pt.y() - prev.y()));
        if (len < 1e-5f)
            continue;
        const float e = segment_e * std::min(1.f, (accumulated + len) / segment_length) - e_used;
        writer.extrude_explicit(pt, e, feedrate);
        accumulated += len;
        e_used += e;
        prev = pt;
    }
}

} // namespace RoundTowerPaths
} // namespace PluginGeo
} // namespace Slic3r
