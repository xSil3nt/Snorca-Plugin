#ifndef slic3r_IWipeTowerShape_hpp_
#define slic3r_IWipeTowerShape_hpp_

#include "AlgorithmProviderRegistry.hpp"
#include "libslic3r/GCode/WipeTower.hpp"
#include "libslic3r/Polygon.hpp"
#include "libslic3r/Point.hpp"

#include <string>
#include <vector>

namespace Slic3r {

class WipeTower2;
class WipeTowerWriter2;

struct WipeTowerWallContext
{
    WipeTower2                       *tower{nullptr};
    WipeTowerWriter2                 *writer{nullptr};
    const WipeTower::box_coordinates *wt_box{nullptr};
    double                            feedrate{0.0};
    bool                              first_layer{false};
    float                             spacing{0.f};
    float                             perimeter_width{0.f};
    std::vector<Vec2f>                skip_points;
    bool                              infill_cone{false};
};

class IWipeTowerShape
{
public:
    virtual ~IWipeTowerShape() = default;
    virtual std::string key() const = 0;
    virtual bool needs_plan_tower_prep() const { return false; }
    virtual void plan_tower_prep(WipeTower2 &tower) { (void) tower; }
    // Built-in shapes extrude inside generate_wall(); plugin shapes return geometry only.
    virtual bool includes_extruded_perimeter() const { return true; }
    // When true, sparse/priming infill is clipped to a circle instead of the rectangular fill box.
    virtual bool get_infill_circle(const WipeTowerWallContext &ctx, Vec2f &center, float &radius) const
    {
        (void) ctx;
        (void) center;
        (void) radius;
        return false;
    }
    virtual Polygon generate_wall(const WipeTowerWallContext &ctx) = 0;
};

using WipeTowerShapeRegistry = AlgorithmProviderRegistry<IWipeTowerShape>;

} // namespace Slic3r

#endif // slic3r_IWipeTowerShape_hpp_
