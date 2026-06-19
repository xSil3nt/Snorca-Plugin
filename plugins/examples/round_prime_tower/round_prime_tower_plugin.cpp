#include "orca/plugin/PluginABI.hpp"
#include "orca/plugin/PluginContext.hpp"
#include "orca/plugin/geometry/Point.hpp"
#include "orca/plugin/geometry/Polygon.hpp"
#include "orca/plugin/geometry/Vec.hpp"

#include <cmath>

using namespace Slic3r;

namespace {

class RoundPrimeTowerShape : public IWipeTowerShape
{
public:
    std::string key() const override { return "round"; }
    bool includes_extruded_perimeter() const override { return false; }

    PluginGeo::Polygon generate_wall(const WipeTowerWallContext &ctx) override
    {
        if (ctx.wt_box == nullptr)
            return {};

        const Vec2f center = (ctx.wt_box->ld + ctx.wt_box->ru) * 0.5f;
        const float width  = ctx.wt_box->ru.x() - ctx.wt_box->ld.x();
        const float height = ctx.wt_box->ru.y() - ctx.wt_box->ld.y();
        const float radius = 0.5f * std::min(width, height);

        return PluginGeo::Polygon::new_scale_circle(center, radius);
    }
};

} // namespace

static void register_round_prime_tower(PluginContext &ctx)
{
    PluginEnumExtension enum_ext;
    enum_ext.option_key  = "wipe_tower_wall_type";
    enum_ext.enum_values = {"round"};
    enum_ext.enum_labels = {"Round"};
    ctx.config_schema().extend_enum(enum_ext);

    ctx.wipe_tower_shapes().register_provider("round", []() { return std::make_unique<RoundPrimeTowerShape>(); });
}

ORCA_PLUGIN_API Slic3r::OrcaPluginDescriptor *orca_plugin_main()
{
    static OrcaPluginDescriptor descriptor{
        "orca.round_prime_tower",
        "Round Prime Tower",
        "1.0.0",
        ORCA_PLUGIN_ABI_VERSION,
        register_round_prime_tower,
    };
    return &descriptor;
}
