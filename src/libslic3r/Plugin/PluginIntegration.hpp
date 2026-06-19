#ifndef slic3r_PluginIntegration_hpp_
#define slic3r_PluginIntegration_hpp_

#include <string>

namespace Slic3r {

class ConfigOption;
struct ConfigOptionDef;

bool plugin_deserialize_extended_enum(ConfigOption *opt, const ConfigOptionDef *optdef, const std::string &value);
std::string plugin_serialize_extended_enum(const ConfigOptionDef &optdef, int int_value);
int plugin_enum_int_from_combo_index(const ConfigOptionDef &opt, int combo_index);
int plugin_enum_combo_index_from_int(const ConfigOptionDef &opt, int enum_int);

} // namespace Slic3r

#endif // slic3r_PluginIntegration_hpp_
