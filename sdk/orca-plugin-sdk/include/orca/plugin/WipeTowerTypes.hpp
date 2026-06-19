#pragma once

#include "geometry/Vec.hpp"

#include <string>
#include <vector>

namespace Slic3r {

struct WipeTowerBoxCoordinates
{
    PluginGeo::Vec2f ld;
    PluginGeo::Vec2f lu;
    PluginGeo::Vec2f rd;
    PluginGeo::Vec2f ru;

    WipeTowerBoxCoordinates() = default;
    WipeTowerBoxCoordinates(float left, float bottom, float width, float height)
        : ld(left, bottom)
        , lu(left, bottom + height)
        , rd(left + width, bottom)
        , ru(left + width, bottom + height)
    {}
    WipeTowerBoxCoordinates(const PluginGeo::Vec2f &pos, float width, float height)
        : WipeTowerBoxCoordinates(pos(0), pos(1), width, height)
    {}

    void translate(const PluginGeo::Vec2f &shift)
    {
        ld += shift;
        lu += shift;
        rd += shift;
        ru += shift;
    }
    void translate(float dx, float dy) { translate(PluginGeo::Vec2f(dx, dy)); }
};

struct WipeTowerHostServices
{
    float (*volume_to_length)(float volume, float lw, float lh){nullptr};
    float (*length_to_volume)(float length, float lw, float lh){nullptr};
    float (*get_wipe_depth)(float volume, float lh, float pw, float extra_flow, float extra_spacing, float width){nullptr};
};

struct WipeTowerLayerContext
{
    float                  z{0.f};
    float                  layer_height{0.f};
    float                  depth{0.f};
    float                  toolchanges_depth{0.f};
    float                  perimeter_width{0.f};
    float                  extrusion_flow{0.f};
    float                  extra_flow{0.f};
    float                  extra_spacing_wipe{0.f};
    bool                   bridging{false};
    float                  feedrate{0.f};
    float                  infill_speed{0.f};
    float                  first_layer_speed{0.f};
    float                  max_purge_speed{0.f};
    bool                   first_layer{false};
    bool                   left_to_right{false};
    WipeTowerHostServices *host{nullptr};
};

struct WipeTowerScaffoldContext
{
    WipeTowerLayerContext   layer;
    WipeTowerBoxCoordinates fill_box;
    bool                    solid_infill{false};
    bool                    adhesion{false};
};

struct WipeTowerToolchangeContext
{
    WipeTowerLayerContext   layer;
    WipeTowerBoxCoordinates cleaning_box;
    float                   wipe_volume{0.f};
    int                     old_tool{0};
    int                     old_temp{0};
    int                     new_temp{0};
    float                   filament_diameter{0.f};
    float                   retract_length{0.f};
    float                   retract_speed{0.f};
    float                   flat_iron_area{0.f};
    bool                    use_gap_wall{false};
};

struct WipeTowerPlanningContext
{
    float wipe_tower_width{0.f};
    float perimeter_width{0.f};
    float layer_height{0.f};
    float extra_flow{0.f};
    float extra_spacing_wipe{0.f};
};

class WipeTower2;
class IWipeTowerPathWriter;

struct WipeTowerWallContext
{
    WipeTower2                       *tower{nullptr};
    IWipeTowerPathWriter              *writer{nullptr};
    void                              *host_native_writer{nullptr};
    WipeTowerHostServices            *host{nullptr};
    const WipeTowerBoxCoordinates    *wt_box{nullptr};
    double                            feedrate{0.0};
    bool                              first_layer{false};
    float                             spacing{0.f};
    float                             perimeter_width{0.f};
    std::vector<PluginGeo::Vec2f>     skip_points;
    bool                              infill_cone{false};
};

} // namespace Slic3r
