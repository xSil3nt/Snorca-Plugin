#include "GCodeTransformRegistry.hpp"

namespace Slic3r {

void GCodeTransformRegistry::register_layer_transform(const std::string &key, LayerGCodeTransformFn fn, const std::string &plugin_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_transforms.push_back({key, std::move(fn), plugin_id});
}

void GCodeTransformRegistry::apply_layer_transforms(LayerGCodeTransformContext &ctx) const
{
    std::vector<LayerGCodeTransformFn> transforms;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        transforms.reserve(m_transforms.size());
        for (const TransformEntry &entry : m_transforms) {
            if (entry.fn)
                transforms.push_back(entry.fn);
        }
    }
    for (const LayerGCodeTransformFn &fn : transforms)
        fn(ctx);
}

} // namespace Slic3r
