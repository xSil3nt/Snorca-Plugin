#include "PluginLoader.hpp"

#include "PluginManager.hpp"

#include "libslic3r/Utils.hpp"

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>

namespace Slic3r {

void load_orca_plugins(const std::vector<std::string> &explicit_paths)
{
    PluginManager &manager = PluginManager::instance();
    manager.initialize();

    for (const std::string &path : explicit_paths) {
        if (path.empty())
            continue;
        std::string error;
        if (!manager.load_plugin_path(path, error))
            BOOST_LOG_TRIVIAL(warning) << "Failed to load plugin from --load-plugin: " << error;
    }

    const std::string plugins_dir = (boost::filesystem::path(get_data_dir()) / "plugins").string();
    manager.load_plugins_from_directory(plugins_dir);
    manager.apply_config_schema();
}

} // namespace Slic3r
