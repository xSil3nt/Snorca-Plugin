#include "orca/plugin/PluginABI.hpp"
#include "orca/plugin/PluginContext.hpp"

using namespace Slic3r;

static void register_plugin(PluginContext &ctx)
{
    (void) ctx;
}

extern "C" Slic3r::OrcaPluginDescriptor *orca_plugin_main()
{
    static OrcaPluginDescriptor descriptor{
        "com.example.myplugin",
        "My Orca Plugin",
        "0.1.0",
        ORCA_PLUGIN_ABI_VERSION,
        register_plugin,
    };
    return &descriptor;
}
