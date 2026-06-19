#ifndef slic3r_PluginIntegration_hpp_
#define slic3r_PluginIntegration_hpp_

#include <string>

namespace Slic3r {

class ConfigOption;
struct ConfigOptionDef;

bool plugin_deserialize_extended_enum(ConfigOption *opt, const ConfigOptionDef *optdef, const std::string &value);

} // namespace Slic3r

#endif // slic3r_PluginIntegration_hpp_
