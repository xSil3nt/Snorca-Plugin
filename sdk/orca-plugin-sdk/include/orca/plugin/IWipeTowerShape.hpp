#pragma once

#include "AlgorithmProviderRegistry.hpp"
#include "IWipeTowerPathWriter.hpp"
#include "WipeTowerTypes.hpp"
#include "geometry/Polygon.hpp"

#include <string>

namespace Slic3r {

class IWipeTowerShape
{
public:
    virtual ~IWipeTowerShape() = default;

    virtual std::string key() const = 0;
    virtual bool        needs_plan_tower_prep() const { return false; }
    virtual void        plan_tower_prep(WipeTower2 &tower) { (void) tower; }
    virtual bool        includes_extruded_perimeter() const { return true; }

    virtual PluginGeo::Polygon generate_wall(const WipeTowerWallContext &ctx) = 0;

    virtual bool generate_sparse_scaffold(const WipeTowerScaffoldContext &ctx, IWipeTowerPathWriter &writer)
    {
        (void) ctx;
        (void) writer;
        return false;
    }
    virtual bool generate_toolchange_wipe(const WipeTowerToolchangeContext &ctx, IWipeTowerPathWriter &writer)
    {
        (void) ctx;
        (void) writer;
        return false;
    }
    virtual bool generate_toolchange_unload(const WipeTowerToolchangeContext &ctx, IWipeTowerPathWriter &writer)
    {
        (void) ctx;
        (void) writer;
        return false;
    }
    virtual bool generate_toolchange_load(const WipeTowerToolchangeContext &ctx, IWipeTowerPathWriter &writer)
    {
        (void) ctx;
        (void) writer;
        return false;
    }
    virtual float get_wipe_channel_width(const WipeTowerPlanningContext &ctx) const
    {
        (void) ctx;
        return 0.f;
    }
};

using WipeTowerShapeRegistry = AlgorithmProviderRegistry<IWipeTowerShape>;

} // namespace Slic3r
