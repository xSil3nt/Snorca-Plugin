#pragma once

#include <orca/plugin/geometry/Polygon.hpp>
#include <orca/plugin/geometry/Vec.hpp>
#include <orca/plugin/WipeTowerTypes.hpp>

#include "libslic3r/GCode/WipeTower.hpp"
#include "libslic3r/Polygon.hpp"

namespace Slic3r {

inline ::Slic3r::Polygon to_host_polygon(const PluginGeo::Polygon &sdk)
{
    ::Slic3r::Polygon host;
    host.points.reserve(sdk.points.size());
    for (const PluginGeo::Point &pt : sdk.points)
        host.points.emplace_back(pt.x(), pt.y());
    return host;
}

inline PluginGeo::Polygon to_sdk_polygon(const ::Slic3r::Polygon &host)
{
    PluginGeo::Polygon sdk;
    sdk.points.reserve(host.points.size());
    for (const ::Slic3r::Point &pt : host.points)
        sdk.points.emplace_back(pt.x(), pt.y());
    return sdk;
}

inline WipeTower::box_coordinates to_host_box(const WipeTowerBoxCoordinates &box)
{
    return WipeTower::box_coordinates(box.ld, box.ru.x() - box.lu.x(), box.ru.y() - box.rd.y());
}

inline WipeTowerBoxCoordinates to_sdk_box(const WipeTower::box_coordinates &box)
{
    return WipeTowerBoxCoordinates(box.ld, box.ru.x() - box.lu.x(), box.ru.y() - box.rd.y());
}

inline std::vector<PluginGeo::Vec2f> to_sdk_skip_points(const std::vector<Vec2f> &host_points)
{
    std::vector<PluginGeo::Vec2f> out;
    out.reserve(host_points.size());
    for (const Vec2f &pt : host_points)
        out.emplace_back(pt.x(), pt.y());
    return out;
}

} // namespace Slic3r
