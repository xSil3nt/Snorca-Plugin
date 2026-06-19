#include "orca/plugin/PluginABI.hpp"
#include "orca/plugin/PluginContext.hpp"

#include "libslic3r/GCode/WipeTower.hpp"
#include "libslic3r/Point.hpp"

#include <cmath>

using namespace Slic3r;

namespace {

class RoundPrimeTowerShape : public IWipeTowerShape
{
public:
    std::string key() const override { return "round"; }
    bool includes_extruded_perimeter() const override { return false; }

    bool get_infill_circle(const WipeTowerWallContext &ctx, Vec2f &center, float &radius) const override
    {
        if (ctx.wt_box == nullptr)
            return false;
        center = (ctx.wt_box->ld + ctx.wt_box->ru) / 2.f;
        const float width  = ctx.wt_box->ru.x() - ctx.wt_box->ld.x();
        const float height = ctx.wt_box->ru.y() - ctx.wt_box->ld.y();
        radius = 0.5f * std::min(width, height) - ctx.perimeter_width;
        return radius > 0.f;
    }

    Polygon generate_wall(const WipeTowerWallContext &ctx) override
    {
        if (ctx.wt_box == nullptr)
            return {};

        const Vec2f center = (ctx.wt_box->ld + ctx.wt_box->ru) / 2.f;
        const float width  = ctx.wt_box->ru.x() - ctx.wt_box->ld.x();
        const float height = ctx.wt_box->ru.y() - ctx.wt_box->ld.y();
        const float radius = 0.5f * std::min(width, height);

        Polygon circle;
        constexpr int segments = 48;
        circle.points.reserve(segments);
        for (int i = 0; i < segments; ++i) {
            const float angle = float(2.0 * M_PI * i / segments);
            circle.points.emplace_back(
                coord_t(scale_(center.x() + radius * std::cos(angle))),
                coord_t(scale_(center.y() + radius * std::sin(angle))));
        }
        return circle;
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
