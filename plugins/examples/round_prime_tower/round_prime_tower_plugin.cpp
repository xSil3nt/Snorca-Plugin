#include "orca/plugin/PluginABI.hpp"
#include "orca/plugin/PluginContext.hpp"
#include "orca/plugin/geometry/Polygon.hpp"
#include "orca/plugin/geometry/Vec.hpp"
#include "orca/plugin/util/RoundTowerPaths.hpp"

#include <cmath>

using namespace Slic3r;
using namespace Slic3r::PluginGeo::RoundTowerPaths;

namespace {

PluginGeo::Vec2f box_center(const WipeTowerBoxCoordinates &box)
{
    return (box.ld + box.ru) * 0.5f;
}

class RoundPrimeTowerShape : public IWipeTowerShape
{
public:
    std::string key() const override { return "round"; }
    bool        includes_extruded_perimeter() const override { return false; }

    float get_wipe_channel_width(const WipeTowerPlanningContext &ctx) const override
    {
        const float inner_r = 0.5f * ctx.wipe_tower_width - 2.f * ctx.perimeter_width;
        return inner_r > 0.f ? 2.f * inner_r : 0.f;
    }

    PluginGeo::Polygon generate_wall(const WipeTowerWallContext &ctx) override
    {
        if (ctx.wt_box == nullptr)
            return {};

        const PluginGeo::Vec2f center = box_center(*ctx.wt_box);
        const float            width  = ctx.wt_box->ru.x() - ctx.wt_box->ld.x();
        const float            height = ctx.wt_box->ru.y() - ctx.wt_box->ld.y();
        const float            radius = 0.5f * std::min(width, height);
        return PluginGeo::Polygon::new_scale_circle(center, radius);
    }

    bool generate_sparse_scaffold(const WipeTowerScaffoldContext &ctx, IWipeTowerPathWriter &writer) override
    {
        const float pw       = ctx.layer.perimeter_width;
        const float feedrate = ctx.layer.feedrate;
        const auto &box      = ctx.fill_box;
        const auto  center   = box_center(box);

        float outer_r = inner_radius_from_box(box.ld, box.ru, 0.f) + pw;
        float inner_r = inner_radius_from_box(box.ld, box.ru, 2.f * pw);
        if (outer_r <= inner_r)
            return true;

        const float ring_step = ctx.solid_infill ? pw : pw * 1.5f;
        const float min_chord = pw - 1e-3f;

        if (ctx.solid_infill && ctx.layer.first_layer) {
            outer_r = std::min(outer_r + pw, 0.5f * std::min(box.ru.x() - box.ld.x(), box.ru.y() - box.ld.y()));
        }

        emit_concentric_rings(center, outer_r, inner_r, ring_step, box.ld, box.ru, min_chord,
                              [&](float x1, float y, float x2) {
                                  writer.extrude(x1, y, feedrate).extrude(x2, y);
                              });
        return true;
    }

    bool generate_toolchange_wipe(const WipeTowerToolchangeContext &ctx, IWipeTowerPathWriter &writer) override
    {
        if (ctx.layer.host == nullptr || ctx.layer.host->volume_to_length == nullptr)
            return false;

        writer.set_extrusion_flow(ctx.layer.extrusion_flow * (ctx.layer.first_layer ? 1.18f : 1.f))
            .append("; CP TOOLCHANGE WIPE\n");
        writer.set_extrusion_flow(ctx.layer.extrusion_flow * ctx.layer.extra_flow);
        const float line_width = ctx.layer.perimeter_width * ctx.layer.extra_flow;
        writer.change_analyzer_line_width(line_width);

        const auto &box    = ctx.cleaning_box;
        const auto  center = box_center(box);
        const float inner_r = inner_radius_from_box(box.ld, box.ru, ctx.layer.perimeter_width);
        const float min_len = ctx.layer.perimeter_width - 1e-3f;

        const float target_speed = ctx.layer.first_layer ?
                                       ctx.layer.first_layer_speed * 60.f :
                                       std::min(ctx.layer.max_purge_speed * 60.f, ctx.layer.infill_speed * 60.f);
        float       wipe_speed   = 0.33f * target_speed;

        float length_needed = ctx.layer.host->volume_to_length(ctx.wipe_volume, ctx.layer.perimeter_width,
                                                               ctx.layer.layer_height) /
                              ctx.layer.extra_flow;
        const float dy = (ctx.layer.first_layer ? ctx.layer.extra_flow : ctx.layer.extra_spacing_wipe) *
                         ctx.layer.perimeter_width;

        bool  left_to_right = ctx.layer.left_to_right;
        float y             = writer.y();
        if (y < box.ld.y() + dy)
            y = box.ld.y() + dy;

        for (int row = 0; length_needed > 1e-3f && y <= box.lu.y() + 1e-3f; ++row) {
            if (row > 0) {
                writer.extrude(writer.x(), y, wipe_speed);
                if (wipe_speed < target_speed)
                    wipe_speed = std::min(target_speed, wipe_speed + 50.f);
            }

            float x_low = 0.f;
            float x_high = 0.f;
            if (!circle_chord_at_y(y, center.x(), center.y(), inner_r, box.ld.x(), box.ru.x(), min_len, x_low, x_high))
                break;

            const float x_target = left_to_right ? x_high : x_low;
            const float dx       = std::abs(x_target - writer.x());
            writer.extrude(x_target, y, wipe_speed);
            length_needed -= dx;
            left_to_right = !left_to_right;
            y += dy;
        }

        writer.set_extrusion_flow(ctx.layer.extrusion_flow);
        writer.change_analyzer_line_width(ctx.layer.perimeter_width);
        return true;
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
