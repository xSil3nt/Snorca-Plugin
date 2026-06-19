#pragma once

namespace Slic3r {

// Mirrors libslic3r ConfigOptionType values used by PluginConfigOptionDef::type.
enum PluginConfigOptionType : int {
    PluginCoFloat   = 1,
    PluginCoInt     = 2,
    PluginCoString  = 3,
    PluginCoPercent = 4,
    PluginCoBool    = 8,
    PluginCoEnum    = 9,
};

} // namespace Slic3r
