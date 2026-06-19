#pragma once

#include <cstdint>
#include <cmath>

namespace Slic3r {
namespace PluginGeo {

using coord_t  = int64_t;
using coordf_t = double;

static constexpr double SCALING_FACTOR_INTERNAL = 0.000001;

inline coordf_t plugin_geo_scale(coordf_t v) { return v / SCALING_FACTOR_INTERNAL; }
inline coordf_t plugin_geo_unscale(coord_t v) { return coordf_t(v) * SCALING_FACTOR_INTERNAL; }

} // namespace PluginGeo
} // namespace Slic3r
