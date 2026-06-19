#include "PluginSliceDataAccess.hpp"

#include "PluginManager.hpp"
#include "SlicingHookBus.hpp"

#include "libslic3r/Layer.hpp"
#include "libslic3r/Surface.hpp"

namespace Slic3r {
namespace {

ExtrusionEntityCollection *collection_for_kind(LayerRegion &region, PluginSliceCollectionKind kind)
{
    switch (kind) {
    case PluginSliceCollectionKind::Perimeters: return &region.perimeters;
    case PluginSliceCollectionKind::Fills: return &region.fills;
    case PluginSliceCollectionKind::ThinFills: return &region.thin_fills;
    default: return nullptr;
    }
}

const ExtrusionEntityCollection *collection_for_kind(const LayerRegion &region, PluginSliceCollectionKind kind)
{
    switch (kind) {
    case PluginSliceCollectionKind::Perimeters: return &region.perimeters;
    case PluginSliceCollectionKind::Fills: return &region.fills;
    case PluginSliceCollectionKind::ThinFills: return &region.thin_fills;
    default: return nullptr;
    }
}

void collect_paths(const ExtrusionEntity *entity, PluginGeo::ExtrusionCollection &sdk)
{
    if (entity == nullptr)
        return;
    if (entity->is_collection()) {
        for (const ExtrusionEntity *child : static_cast<const ExtrusionEntityCollection *>(entity)->entities)
            collect_paths(child, sdk);
        return;
    }
    if (entity->is_loop()) {
        const ExtrusionLoop *loop = static_cast<const ExtrusionLoop *>(entity);
        for (const ExtrusionPath &path : loop->paths) {
            PluginGeo::ExtrusionPath sdk_path;
            sdk_path.polyline.points.reserve(path.polyline.points.size());
            for (const Point &pt : path.polyline.points)
                sdk_path.polyline.points.emplace_back(pt.x(), pt.y());
            sdk_path.width  = float(path.width);
            sdk_path.height = float(path.height);
            sdk_path.role   = to_sdk_extrusion_role(path.role());
            sdk.append(std::move(sdk_path));
        }
        return;
    }
    if (const ExtrusionPath *path = dynamic_cast<const ExtrusionPath *>(entity)) {
        PluginGeo::ExtrusionPath sdk_path;
        sdk_path.polyline.points.reserve(path->polyline.points.size());
        for (const Point &pt : path->polyline.points)
            sdk_path.polyline.points.emplace_back(pt.x(), pt.y());
        sdk_path.width  = float(path->width);
        sdk_path.height = float(path->height);
        sdk_path.role   = to_sdk_extrusion_role(path->role());
        sdk.append(std::move(sdk_path));
    }
}

} // namespace

PluginGeo::ExtrusionRole to_sdk_extrusion_role(ExtrusionRole role)
{
    switch (role) {
    case erPerimeter: return PluginGeo::ExtrusionRole::Perimeter;
    case erExternalPerimeter: return PluginGeo::ExtrusionRole::ExternalPerimeter;
    case erOverhangPerimeter: return PluginGeo::ExtrusionRole::OverhangPerimeter;
    case erInternalInfill: return PluginGeo::ExtrusionRole::InternalInfill;
    case erSolidInfill:
    case erTopSolidInfill: return PluginGeo::ExtrusionRole::SolidInfill;
    case erSupportMaterial:
    case erSupportMaterialInterface: return PluginGeo::ExtrusionRole::SupportMaterial;
    default: return PluginGeo::ExtrusionRole::Custom;
    }
}

ExtrusionRole to_host_extrusion_role(PluginGeo::ExtrusionRole role)
{
    switch (role) {
    case PluginGeo::ExtrusionRole::Perimeter: return erPerimeter;
    case PluginGeo::ExtrusionRole::ExternalPerimeter: return erExternalPerimeter;
    case PluginGeo::ExtrusionRole::OverhangPerimeter: return erOverhangPerimeter;
    case PluginGeo::ExtrusionRole::InternalInfill: return erInternalInfill;
    case PluginGeo::ExtrusionRole::SolidInfill:
    case PluginGeo::ExtrusionRole::TopSolidInfill: return erSolidInfill;
    case PluginGeo::ExtrusionRole::SupportMaterial: return erSupportMaterial;
    default: return erCustom;
    }
}

void to_sdk_extrusion_collection(const ExtrusionEntityCollection &host, PluginGeo::ExtrusionCollection &sdk)
{
    sdk.clear();
    for (const ExtrusionEntity *entity : host.entities)
        collect_paths(entity, sdk);
}

void apply_sdk_extrusion_collection(ExtrusionEntityCollection &host, const PluginGeo::ExtrusionCollection &sdk)
{
    host.clear();
    for (const PluginGeo::ExtrusionPath &sdk_path : sdk.paths) {
        if (sdk_path.polyline.points.size() < 2)
            continue;
        auto *host_path = new ExtrusionPath();
        host_path->polyline.points.reserve(sdk_path.polyline.points.size());
        for (const PluginGeo::Point &pt : sdk_path.polyline.points)
            host_path->polyline.points.emplace_back(pt.x(), pt.y());
        host_path->width  = sdk_path.width;
        host_path->height = sdk_path.height;
        host_path->set_extrusion_role(to_host_extrusion_role(sdk_path.role));
        host.entities.emplace_back(host_path);
    }
}

bool HostConfigReader::has(const std::string &key) const
{
    return m_config != nullptr && m_config->has(key);
}

std::string HostConfigReader::get_string(const std::string &key) const
{
    if (m_config == nullptr || !m_config->has(key))
        return {};
    const ConfigOption *opt = m_config->option(key);
    return opt ? opt->serialize() : std::string{};
}

double HostConfigReader::get_float(const std::string &key) const
{
    if (m_config == nullptr || !m_config->has(key))
        return 0.;
    const ConfigOption *opt = m_config->option(key);
    if (opt == nullptr)
        return 0.;
    if (opt->type() == coFloat || opt->type() == coPercent)
        return static_cast<const ConfigOptionFloat *>(opt)->value;
    return 0.;
}

int HostConfigReader::get_int(const std::string &key) const
{
    if (m_config == nullptr || !m_config->has(key))
        return 0;
    const ConfigOption *opt = m_config->option(key);
    if (opt == nullptr)
        return 0;
    if (opt->type() == coBool)
        return static_cast<const ConfigOptionBool *>(opt)->value ? 1 : 0;
    if (opt->type() == coInt)
        return static_cast<const ConfigOptionInt *>(opt)->value;
    if (opt->type() == coEnum)
        return opt->getInt();
    return 0;
}

HostSliceData::HostSliceData(PrintObject *print_object, const ConfigBase *config)
    : m_print_object(print_object)
    , m_config_reader(config)
{}

size_t HostSliceData::layer_count() const
{
    return m_print_object ? m_print_object->layer_count() : 0;
}

size_t HostSliceData::region_count() const
{
    return m_print_object ? m_print_object->num_printing_regions() : 0;
}

bool HostSliceData::get_extrusion_collection(size_t layer_id,
                                             size_t region_id,
                                             PluginSliceCollectionKind kind,
                                             PluginGeo::ExtrusionCollection &out) const
{
    if (m_print_object == nullptr || layer_id >= m_print_object->layer_count())
        return false;
    Layer *layer = m_print_object->get_layer(layer_id);
    if (region_id >= layer->regions().size())
        return false;
    const ExtrusionEntityCollection *collection = collection_for_kind(*layer->regions()[region_id], kind);
    if (collection == nullptr)
        return false;
    to_sdk_extrusion_collection(*collection, out);
    return true;
}

bool HostSliceData::set_extrusion_collection(size_t layer_id,
                                             size_t region_id,
                                             PluginSliceCollectionKind kind,
                                             const PluginGeo::ExtrusionCollection &in)
{
    if (m_print_object == nullptr || layer_id >= m_print_object->layer_count())
        return false;
    Layer *layer = m_print_object->get_layer(layer_id);
    if (region_id >= layer->regions().size())
        return false;
    ExtrusionEntityCollection *collection = collection_for_kind(*layer->regions()[region_id], kind);
    if (collection == nullptr)
        return false;
    apply_sdk_extrusion_collection(*collection, in);
    return true;
}

std::vector<PluginGeo::Polygon> HostSliceData::get_region_slices(size_t layer_id, size_t region_id) const
{
    std::vector<PluginGeo::Polygon> out;
    if (m_print_object == nullptr || layer_id >= m_print_object->layer_count())
        return out;
    const Layer *layer = m_print_object->get_layer(layer_id);
    if (region_id >= layer->regions().size())
        return out;
    const LayerRegion &region = *layer->regions()[region_id];
    for (const Surface &surface : region.slices.surfaces) {
        PluginGeo::Polygon sdk_poly;
        sdk_poly.points.reserve(surface.expolygon.contour.points.size());
        for (const Point &pt : surface.expolygon.contour.points)
            sdk_poly.points.emplace_back(pt.x(), pt.y());
        if (sdk_poly.is_valid())
            out.push_back(std::move(sdk_poly));
    }
    return out;
}

void fire_print_object_slicing_hook(PrintObject *print_object, PrintObjectStep object_step, SlicingHookPhase phase)
{
    const ConfigBase *config = nullptr;
    if (print_object != nullptr && print_object->print() != nullptr)
        config = static_cast<const ConfigBase *>(&print_object->print()->config());
    HostSliceData      slice_data(print_object, config);
    SlicingHookContext ctx = make_slicing_hook_context(print_object, object_step);
    ctx.slice_data         = &slice_data;
    PluginManager::instance().slicing_hooks().fire(phase, ctx);
}

} // namespace Slic3r
