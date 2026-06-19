#pragma once

#include "geometry/Vec.hpp"

#include <string>

namespace Slic3r {

class IWipeTowerPathWriter
{
public:
    virtual ~IWipeTowerPathWriter() = default;

    virtual IWipeTowerPathWriter &travel(float x, float y, float feedrate = 0.f) = 0;
    virtual IWipeTowerPathWriter &travel(const PluginGeo::Vec2f &dest, float feedrate = 0.f) = 0;
    virtual IWipeTowerPathWriter &extrude(float x, float y, float feedrate = 0.f) = 0;
    virtual IWipeTowerPathWriter &extrude(const PluginGeo::Vec2f &dest, float feedrate = 0.f) = 0;
    virtual IWipeTowerPathWriter &extrude_explicit(float x, float y, float e, float feedrate = 0.f) = 0;
    virtual IWipeTowerPathWriter &extrude_explicit(const PluginGeo::Vec2f &dest, float e, float feedrate = 0.f) = 0;
    virtual IWipeTowerPathWriter &ram(float x1, float x2, float dy, float e0, float e, float feedrate) = 0;
    virtual IWipeTowerPathWriter &retract(float e, float feedrate = 0.f) = 0;
    virtual IWipeTowerPathWriter &load(float e, float feedrate = 0.f) = 0;

    virtual float                       x() const = 0;
    virtual float                       y() const = 0;
    virtual const PluginGeo::Vec2f     &pos() const = 0;
    virtual IWipeTowerPathWriter       &feedrate(float feedrate) = 0;

    virtual IWipeTowerPathWriter &append(const std::string &text) = 0;
    virtual IWipeTowerPathWriter &comment_with_value(const char *comment, int value) = 0;

    virtual IWipeTowerPathWriter &rectangle(const PluginGeo::Vec2f &ld, float width, float height, float feedrate = 0.f) = 0;
    virtual IWipeTowerPathWriter &suppress_preview() = 0;
    virtual IWipeTowerPathWriter &resume_preview() = 0;
    virtual IWipeTowerPathWriter &change_analyzer_line_width(float line_width) = 0;
    virtual IWipeTowerPathWriter &set_extrusion_flow(float flow) = 0;
    virtual IWipeTowerPathWriter &add_wipe_point(float x, float y) = 0;
    virtual IWipeTowerPathWriter &add_wipe_point(const PluginGeo::Vec2f &pt) = 0;

    virtual IWipeTowerPathWriter &set_extruder_temp(int temperature, bool wait = false) = 0;
    virtual IWipeTowerPathWriter &disable_linear_advance() = 0;
    virtual void spiral_flat_ironing(const PluginGeo::Vec2f &center, float area, float step_length, float feedrate) = 0;
};

} // namespace Slic3r
