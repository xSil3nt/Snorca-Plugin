#pragma once

#include "SliceDataAccess.hpp"

#include <functional>
#include <mutex>
#include <string>
#include <vector>

namespace Slic3r {

struct LayerGCodeTransformContext
{
    std::string          gcode;
    size_t               layer_id{0};
    double               layer_z{0.};
    IPluginConfigReader *config{nullptr};
};

using LayerGCodeTransformFn = std::function<void(LayerGCodeTransformContext &)>;

class GCodeTransformRegistry
{
public:
    void register_layer_transform(const std::string &key, LayerGCodeTransformFn fn, const std::string &plugin_id = {});
    void apply_layer_transforms(LayerGCodeTransformContext &ctx) const;

private:
    struct TransformEntry
    {
        std::string           key;
        LayerGCodeTransformFn fn;
        std::string           plugin_id;
    };

    mutable std::mutex        m_mutex;
    std::vector<TransformEntry> m_transforms;
};

} // namespace Slic3r
