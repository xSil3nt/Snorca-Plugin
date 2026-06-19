#pragma once

#include "GCodeTransformRegistry.hpp"
#include "PluginManager.hpp"
#include "PluginSliceDataAccess.hpp"

#include "libslic3r/GCode.hpp"
#include "libslic3r/Print.hpp"

namespace Slic3r {

inline LayerResult apply_plugin_layer_gcode_transforms(const Print &print, LayerResult in)
{
    if (in.nop_layer_result || in.gcode.empty())
        return in;
    HostConfigReader reader(static_cast<const ConfigBase *>(&print.config()));
    LayerGCodeTransformContext ctx;
    ctx.gcode     = std::move(in.gcode);
    ctx.layer_id  = in.layer_id;
    ctx.config    = &reader;
    PluginManager::instance().gcode_transforms().apply_layer_transforms(ctx);
    in.gcode = std::move(ctx.gcode);
    return in;
}

} // namespace Slic3r
