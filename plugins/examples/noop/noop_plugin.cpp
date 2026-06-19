#include "orca/plugin/PluginABI.hpp"
#include "orca/plugin/PluginContext.hpp"

using namespace Slic3r;

static void register_noop_plugin(PluginContext &ctx)
{
    (void) ctx;
}

extern "C" Slic3r::OrcaPluginDescriptor *orca_plugin_main()
{
    static OrcaPluginDescriptor descriptor{
        "orca.noop",
        "Orca No-op Plugin",
        "1.0.0",
        ORCA_PLUGIN_ABI_VERSION,
        register_noop_plugin,
    };
    return &descriptor;
}
