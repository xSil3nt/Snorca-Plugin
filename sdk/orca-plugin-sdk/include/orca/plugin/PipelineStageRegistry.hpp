#pragma once

#include "geometry/ExtrusionTypes.hpp"
#include "geometry/Polygon.hpp"
#include "SliceDataAccess.hpp"

#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <utility>

namespace Slic3r {

struct PipelineStageContext
{
    std::string                      stage_id;
    std::string                      handler_key;
    IPluginConfigReader             *config{nullptr};
    std::vector<PluginGeo::Polygon>  input_polygons;
    PluginGeo::ExtrusionCollection   output;
    double                           layer_height{0.};
    double                           layer_z{0.};
    size_t                           layer_id{0};
    float                            infill_density{0.f};
    float                            infill_angle{0.f};
};

using PipelineStageHandler = std::function<bool(PipelineStageContext &)>;

class PipelineStageRegistry
{
public:
    void register_handler(const std::string &stage_id, const std::string &key, PipelineStageHandler handler);
    bool run(const std::string &stage_id, const std::string &key, PipelineStageContext &ctx) const;
    bool has(const std::string &stage_id, const std::string &key) const;

    using HandlerSyncFn = std::function<void(const std::string &stage_id, const std::string &key)>;
    void set_handler_sync_callback(HandlerSyncFn fn) { m_sync_callback = std::move(fn); }

private:
    using HandlerMap = std::map<std::string, PipelineStageHandler>;

    mutable std::mutex              m_mutex;
    std::map<std::string, HandlerMap> m_handlers;
    HandlerSyncFn                     m_sync_callback;
};

} // namespace Slic3r
