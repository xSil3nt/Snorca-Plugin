#include "PipelineStageRegistry.hpp"

#include "InfillProviderRegistryImpl.hpp"
#include "PluginManager.hpp"
#include "PluginSliceDataAccess.hpp"
#include "WallGeneratorRegistry.hpp"

#include "libslic3r/Fill/FillBase.hpp"
#include "libslic3r/PerimeterGenerator.hpp"

#include <memory>

namespace Slic3r {
namespace {

class PipelineWallGenerator : public IWallGenerator
{
public:
    explicit PipelineWallGenerator(std::string key) : m_key(std::move(key)) {}

    std::string key() const override { return m_key; }
    void        process(PerimeterGenerator &generator) override;

private:
    std::string m_key;
};

class PipelineInfillFill : public Fill
{
public:
    explicit PipelineInfillFill(std::string key) : m_key(std::move(key)) {}

    Fill *clone() const override { return new PipelineInfillFill(m_key); }
    bool  is_self_crossing() override { return false; }

    void fill_surface_extrusion(const Surface *surface, const FillParams &params, ExtrusionEntitiesPtr &out) override;

private:
    std::string m_key;
};

void sync_pipeline_handler(const std::string &stage_id, const std::string &key)
{
    auto &pm = PluginManager::instance();
    if (stage_id == "perimeters") {
        pm.wall_generators().register_provider(key, [key]() { return std::make_unique<PipelineWallGenerator>(key); });
    } else if (stage_id == "infill") {
        pm.infill_providers().register_provider(key, [key]() { return new PipelineInfillFill(key); });
    }
}

} // namespace

bool PipelineStageRegistry::run(const std::string &stage_id, const std::string &key, PipelineStageContext &ctx) const
{
    PipelineStageHandler handler;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto stage_it = m_handlers.find(stage_id);
        if (stage_it == m_handlers.end())
            return false;
        auto handler_it = stage_it->second.find(key);
        if (handler_it == stage_it->second.end())
            return false;
        handler = handler_it->second;
    }
    return handler ? handler(ctx) : false;
}

bool PipelineStageRegistry::has(const std::string &stage_id, const std::string &key) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto stage_it = m_handlers.find(stage_id);
    if (stage_it == m_handlers.end())
        return false;
    return stage_it->second.find(key) != stage_it->second.end();
}

void PipelineWallGenerator::process(PerimeterGenerator &generator)
{
    PipelineStageContext ctx;
    ctx.stage_id    = "perimeters";
    ctx.handler_key = m_key;
    ctx.layer_id    = size_t(generator.layer_id);
    ctx.layer_height = generator.layer_height;
    ctx.layer_z     = generator.slice_z;

    if (generator.slices != nullptr) {
        for (const Surface &surface : generator.slices->surfaces) {
            PluginGeo::Polygon sdk_poly;
            sdk_poly.points.reserve(surface.expolygon.contour.points.size());
            for (const Point &pt : surface.expolygon.contour.points)
                sdk_poly.points.emplace_back(pt.x(), pt.y());
            if (sdk_poly.is_valid())
                ctx.input_polygons.push_back(std::move(sdk_poly));
        }
    }

    if (!PluginManager::instance().pipeline_stages().run("perimeters", m_key, ctx))
        return;

    if (ctx.output.empty() || generator.loops == nullptr)
        return;

    generator.loops->clear();
    for (const PluginGeo::ExtrusionPath &sdk_path : ctx.output.paths) {
        if (sdk_path.polyline.points.size() < 2)
            continue;
        ExtrusionPath host_path;
        host_path.polyline.points.reserve(sdk_path.polyline.points.size());
        for (const PluginGeo::Point &pt : sdk_path.polyline.points)
            host_path.polyline.points.emplace_back(pt.x(), pt.y());
        host_path.width  = sdk_path.width;
        host_path.height = sdk_path.height;
        host_path.set_extrusion_role(to_host_extrusion_role(sdk_path.role));
        generator.loops->append(host_path);
    }
}

void PipelineInfillFill::fill_surface_extrusion(const Surface *surface, const FillParams &params, ExtrusionEntitiesPtr &out)
{
    if (surface == nullptr)
        return;

    PipelineStageContext ctx;
    ctx.stage_id       = "infill";
    ctx.handler_key    = m_key;
    ctx.layer_id       = layer_id;
    ctx.layer_z        = z;
    ctx.layer_height   = params.layer_height;
    ctx.infill_density = params.density;
    ctx.infill_angle   = angle;

    PluginGeo::Polygon sdk_poly;
    sdk_poly.points.reserve(surface->expolygon.contour.points.size());
    for (const Point &pt : surface->expolygon.contour.points)
        sdk_poly.points.emplace_back(pt.x(), pt.y());
    if (sdk_poly.is_valid())
        ctx.input_polygons.push_back(std::move(sdk_poly));

    if (!PluginManager::instance().pipeline_stages().run("infill", m_key, ctx))
        return Fill::fill_surface_extrusion(surface, params, out);

    for (const PluginGeo::ExtrusionPath &sdk_path : ctx.output.paths) {
        if (sdk_path.polyline.points.size() < 2)
            continue;
        auto *host_path = new ExtrusionPath();
        host_path->polyline.points.reserve(sdk_path.polyline.points.size());
        for (const PluginGeo::Point &pt : sdk_path.polyline.points)
            host_path->polyline.points.emplace_back(pt.x(), pt.y());
        host_path->width  = sdk_path.width > 0.f ? sdk_path.width : float(params.flow.scaled_width());
        host_path->height = sdk_path.height > 0.f ? sdk_path.height : float(scale_(params.flow.height()));
        host_path->set_extrusion_role(params.extrusion_role);
        out.emplace_back(host_path);
    }
}

void register_pipeline_stage_sync()
{
    PluginManager::instance().pipeline_stages().set_handler_sync_callback(sync_pipeline_handler);
}

} // namespace Slic3r
