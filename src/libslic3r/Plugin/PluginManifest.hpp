#ifndef slic3r_PluginManifest_hpp_
#define slic3r_PluginManifest_hpp_

#include <string>
#include <vector>

namespace Slic3r {

struct PluginManifest
{
    std::string              id;
    std::string              name;
    std::string              version;
    std::string              author;
    std::string              min_app_version;
    std::string              max_app_version;
    int                      abi_version{0};
    std::string              entry_library;
    std::vector<std::string> config_keys;
    std::vector<std::string> preset_types;
    std::vector<std::string> invalidation_keys;
    std::vector<std::string> extension_points;

    static bool load_from_file(const std::string &path, PluginManifest &out, std::string &error);
    static bool load_from_json_string(const std::string &json_text, PluginManifest &out, std::string &error);
};

} // namespace Slic3r

#endif // slic3r_PluginManifest_hpp_
