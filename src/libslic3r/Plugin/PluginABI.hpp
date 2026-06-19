#ifndef slic3r_PluginABI_hpp_
#define slic3r_PluginABI_hpp_

#include <cstdint>

namespace Slic3r {

class PluginContext;

// Bump when the plugin registration API or host contract changes.
static constexpr int ORCA_PLUGIN_ABI_VERSION = 1;

struct OrcaPluginDescriptor
{
    const char *id;
    const char *name;
    const char *version;
    int         abi_version;
    void (*register_plugin)(PluginContext &ctx);
};

} // namespace Slic3r

extern "C" {
typedef Slic3r::OrcaPluginDescriptor *(*OrcaPluginMainFn)();
}

#endif // slic3r_PluginABI_hpp_
