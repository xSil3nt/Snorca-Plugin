#include "orca/plugin/PluginABI.hpp"
#include "orca/plugin/PluginContext.hpp"
#include "orca/plugin/geometry/ExtrusionTypes.hpp"
#include "orca/plugin/geometry/Point.hpp"

#include <cmath>

using namespace Slic3r;

namespace {

bool generate_crosshatch_infill(PipelineStageContext &ctx)
{
    if (ctx.input_polygons.empty())
        return false;

    PluginGeo::Polygon bounds;
    bool               has_bounds = false;
    for (const PluginGeo::Polygon &poly : ctx.input_polygons) {
        for (const PluginGeo::Point &pt : poly.points) {
            if (!has_bounds) {
                bounds.points = {pt, pt, pt};
                has_bounds    = true;
            } else {
                bounds.points[0] = PluginGeo::Point(std::min(bounds.points[0].x(), pt.x()), std::min(bounds.points[0].y(), pt.y()));
                bounds.points[1] = PluginGeo::Point(std::max(bounds.points[1].x(), pt.x()), std::max(bounds.points[1].y(), pt.y()));
            }
        }
    }
    if (!has_bounds)
        return false;

    const coord_t min_x = bounds.points[0].x();
    const coord_t min_y = bounds.points[0].y();
    const coord_t max_x = bounds.points[1].x();
    const coord_t max_y = bounds.points[1].y();
    const float   spacing = ctx.layer_height > 0. ? float(ctx.layer_height * 1000000.) : 1000000.f;
    const float   angle   = ctx.infill_angle;

    auto emit_line = [&](coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
        PluginGeo::ExtrusionPath path;
        path.role = PluginGeo::ExtrusionRole::InternalInfill;
        path.polyline.points.emplace_back(x0, y0);
        path.polyline.points.emplace_back(x1, y1);
        ctx.output.append(std::move(path));
    };

    const float ca = std::cos(angle);
    const float sa = std::sin(angle);
    for (coord_t d = min_x; d <= max_x; d += coord_t(spacing)) {
        emit_line(d, min_y, coord_t(d + (max_y - min_y) * sa), max_y);
        emit_line(d, max_y, coord_t(d + (max_y - min_y) * sa), min_y);
    }
    (void) ca;

    return !ctx.output.empty();
}

void register_custom_infill_plugin(PluginContext &ctx)
{
    PluginEnumExtension ext;
    ext.option_key    = "sparse_infill_pattern";
    ext.enum_values   = {"plugin_crosshatch"};
    ext.enum_labels   = {"Plugin crosshatch"};
    ctx.config_schema().extend_enum(ext);
    ctx.pipeline_stages().register_handler("infill", "plugin_crosshatch", generate_crosshatch_infill);
}

} // namespace

ORCA_PLUGIN_API Slic3r::OrcaPluginDescriptor *orca_plugin_main()
{
    static OrcaPluginDescriptor descriptor{
        "orca.custom_infill",
        "Custom Crosshatch Infill",
        "1.0.0",
        ORCA_PLUGIN_ABI_VERSION,
        register_custom_infill_plugin,
    };
    return &descriptor;
}
