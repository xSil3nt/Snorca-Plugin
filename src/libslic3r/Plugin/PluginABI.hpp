#ifndef slic3r_PluginABI_hpp_
#define slic3r_PluginABI_hpp_

#include <cstdint>

namespace Slic3r {

class PluginContext;

// Bump when the plugin registration API or host contract changes.
static constexpr int ORCA_PLUGIN_ABI_VERSION = 1;

#if defined(_WIN32)
#  ifdef ORCA_PLUGIN_EXPORTS
#    define ORCA_PLUGIN_API extern "C" __declspec(dllexport)
#  else
#    define ORCA_PLUGIN_API extern "C"
#  endif
#else
#  define ORCA_PLUGIN_API extern "C" __attribute__((visibility("default")))
#endif

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
