#pragma once

#include "Point.hpp"

#include <cstdint>
#include <vector>

namespace Slic3r {
namespace PluginGeo {

using Points = std::vector<Point>;

enum class ExtrusionRole : uint8_t {
    None,
    Perimeter,
    ExternalPerimeter,
    OverhangPerimeter,
    InternalInfill,
    SolidInfill,
    TopSolidInfill,
    SupportMaterial,
    Custom,
};

struct Polyline
{
    Points points;
};

struct ExtrusionPath
{
    Polyline       polyline;
    float          width{0.f};
    float          height{0.f};
    float          speed{0.f};
    ExtrusionRole  role{ExtrusionRole::None};
};

class ExtrusionCollection
{
public:
    std::vector<ExtrusionPath> paths;

    void clear() { paths.clear(); }
    bool empty() const { return paths.empty(); }
    void append(ExtrusionPath path) { paths.push_back(std::move(path)); }
};

} // namespace PluginGeo
} // namespace Slic3r
