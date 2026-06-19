#include "PluginManifest.hpp"

#include "nlohmann/json.hpp"

#include <boost/nowide/fstream.hpp>

namespace Slic3r {

static void read_string_array(const nlohmann::json &j, const char *key, std::vector<std::string> &out)
{
    if (!j.contains(key))
        return;
    const auto &value = j.at(key);
    if (!value.is_array())
        return;
    for (const auto &item : value) {
        if (item.is_string())
            out.emplace_back(item.get<std::string>());
    }
}

bool PluginManifest::load_from_json_string(const std::string &json_text, PluginManifest &out, std::string &error)
{
    try {
        nlohmann::json j = nlohmann::json::parse(json_text);
        if (!j.is_object()) {
            error = "manifest root must be an object";
            return false;
        }
        auto require_string = [&](const char *key, std::string &target) -> bool {
            if (!j.contains(key) || !j.at(key).is_string()) {
                error = std::string("manifest missing string field: ") + key;
                return false;
            }
            target = j.at(key).get<std::string>();
            return true;
        };

        if (!require_string("id", out.id))
            return false;
        if (!require_string("name", out.name))
            return false;
        if (!require_string("version", out.version))
            return false;
        if (!require_string("entry_library", out.entry_library))
            return false;

        if (j.contains("author") && j.at("author").is_string())
            out.author = j.at("author").get<std::string>();
        if (j.contains("min_app_version") && j.at("min_app_version").is_string())
            out.min_app_version = j.at("min_app_version").get<std::string>();
        if (j.contains("max_app_version") && j.at("max_app_version").is_string())
            out.max_app_version = j.at("max_app_version").get<std::string>();
        if (j.contains("abi_version"))
            out.abi_version = j.at("abi_version").get<int>();

        read_string_array(j, "config_keys", out.config_keys);
        read_string_array(j, "preset_types", out.preset_types);
        read_string_array(j, "invalidation_keys", out.invalidation_keys);
        read_string_array(j, "extension_points", out.extension_points);
        return true;
    } catch (const std::exception &ex) {
        error = ex.what();
        return false;
    }
}

bool PluginManifest::load_from_file(const std::string &path, PluginManifest &out, std::string &error)
{
    boost::nowide::ifstream ifs(path);
    if (!ifs.good()) {
        error = "failed to open manifest: " + path;
        return false;
    }
    std::string json_text((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    return load_from_json_string(json_text, out, error);
}

} // namespace Slic3r
