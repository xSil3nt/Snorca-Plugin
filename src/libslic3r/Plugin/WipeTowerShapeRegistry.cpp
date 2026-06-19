#include "WipeTowerShapeRegistry.hpp"

#include "PluginManager.hpp"
#include "libslic3r/GCode/WipeTower2.hpp"
#include "libslic3r/PrintConfig.hpp"

#include <cmath>

namespace Slic3r {

namespace {

class BuiltinRectangleShape : public IWipeTowerShape
{
public:
    std::string key() const override { return "rectangle"; }
    Polygon generate_wall(const WipeTowerWallContext &ctx) override
    {
        return ctx.tower->generate_support_rib_wall(*ctx.writer, *ctx.wt_box, ctx.feedrate, ctx.first_layer, false, true, ctx.skip_points);
    }
};

class BuiltinConeShape : public IWipeTowerShape
{
public:
    std::string key() const override { return "cone"; }
    Polygon generate_wall(const WipeTowerWallContext &ctx) override
    {
        return ctx.tower->generate_support_cone_wall(*ctx.writer, *ctx.wt_box, ctx.feedrate, ctx.infill_cone, ctx.spacing, ctx.skip_points);
    }
};

class BuiltinRibShape : public IWipeTowerShape
{
public:
    std::string key() const override { return "rib"; }
    bool needs_plan_tower_prep() const override { return true; }
    void plan_tower_prep(WipeTower2 &tower) override { tower.plan_tower_for_rib_shape(); }
    Polygon generate_wall(const WipeTowerWallContext &ctx) override
    {
        return ctx.tower->generate_support_rib_wall(*ctx.writer, *ctx.wt_box, ctx.feedrate, ctx.first_layer, true, true, ctx.skip_points);
    }
};

} // namespace

WipeTowerShapeRegistry &wipe_tower_shape_registry()
{
    return PluginManager::instance().wipe_tower_shapes();
}

void register_builtin_wipe_tower_shapes()
{
    auto &registry = wipe_tower_shape_registry();
    registry.register_provider("rectangle", []() { return std::make_unique<BuiltinRectangleShape>(); });
    registry.register_provider("cone", []() { return std::make_unique<BuiltinConeShape>(); });
    registry.register_provider("rib", []() { return std::make_unique<BuiltinRibShape>(); });
}

std::string wipe_tower_wall_type_key(int wall_type_int)
{
    const t_config_enum_values &enum_map = ConfigOptionEnum<WipeTowerWallType>::get_enum_values();
    for (const auto &kv : enum_map) {
        if (kv.second == wall_type_int)
            return kv.first;
    }
    std::string plugin_key = PluginManager::instance().enum_key_for_value("wipe_tower_wall_type", wall_type_int);
    if (!plugin_key.empty())
        return plugin_key;
    return "rectangle";
}

int wipe_tower_wall_type_from_key(const std::string &key)
{
    const t_config_enum_values &enum_map = ConfigOptionEnum<WipeTowerWallType>::get_enum_values();
    auto it = enum_map.find(key);
    if (it != enum_map.end())
        return it->second;
    int plugin_value = PluginManager::instance().enum_value_for_key("wipe_tower_wall_type", key);
    return plugin_value >= 0 ? plugin_value : int(wtwRectangle);
}

} // namespace Slic3r
