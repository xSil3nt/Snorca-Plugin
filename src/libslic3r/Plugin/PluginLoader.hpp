#ifndef slic3r_PluginLoader_hpp_
#define slic3r_PluginLoader_hpp_

#include <string>
#include <vector>

namespace Slic3r {

void load_orca_plugins(const std::vector<std::string> &explicit_paths = {});

} // namespace Slic3r

#endif // slic3r_PluginLoader_hpp_
