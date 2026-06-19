#pragma once

#include "Types.hpp"

#include <Eigen/Core>

namespace Slic3r {
namespace PluginGeo {

using Vec2crd = Eigen::Matrix<coord_t, 2, 1, Eigen::DontAlign>;
using Vec2f   = Eigen::Matrix<float, 2, 1, Eigen::DontAlign>;
using Vec2d   = Eigen::Matrix<double, 2, 1, Eigen::DontAlign>;

} // namespace PluginGeo

using PluginGeo::Vec2f;
using PluginGeo::Vec2crd;
using PluginGeo::Vec2d;

} // namespace Slic3r
