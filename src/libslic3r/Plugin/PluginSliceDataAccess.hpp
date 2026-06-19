#pragma once

#include "SliceDataAccess.hpp"

#include "libslic3r/Config.hpp"
#include "libslic3r/ExtrusionEntity.hpp"
#include "libslic3r/Print.hpp"

#include <orca/plugin/geometry/ExtrusionTypes.hpp>
#include <orca/plugin/geometry/Polygon.hpp>

namespace Slic3r {

PluginGeo::ExtrusionRole to_sdk_extrusion_role(ExtrusionRole role);
ExtrusionRole              to_host_extrusion_role(PluginGeo::ExtrusionRole role);

void to_sdk_extrusion_collection(const ExtrusionEntityCollection &host, PluginGeo::ExtrusionCollection &sdk);
void apply_sdk_extrusion_collection(ExtrusionEntityCollection &host, const PluginGeo::ExtrusionCollection &sdk);

class HostConfigReader : public IPluginConfigReader
{
public:
    explicit HostConfigReader(const ConfigBase *config) : m_config(config) {}

    bool        has(const std::string &key) const override;
    std::string get_string(const std::string &key) const override;
    double      get_float(const std::string &key) const override;
    int         get_int(const std::string &key) const override;

private:
    const ConfigBase *m_config;
};

class HostSliceData : public ISliceData
{
public:
    HostSliceData(PrintObject *print_object, const ConfigBase *config);

    IPluginConfigReader *config() override { return &m_config_reader; }
    size_t               layer_count() const override;
    size_t               region_count() const override;

    bool get_extrusion_collection(size_t layer_id,
                                  size_t region_id,
                                  PluginSliceCollectionKind kind,
                                  PluginGeo::ExtrusionCollection &out) const override;

    bool set_extrusion_collection(size_t layer_id,
                                  size_t region_id,
                                  PluginSliceCollectionKind kind,
                                  const PluginGeo::ExtrusionCollection &in) override;

    std::vector<PluginGeo::Polygon> get_region_slices(size_t layer_id, size_t region_id) const override;

private:
    PrintObject      *m_print_object;
    HostConfigReader  m_config_reader;
};

void fire_print_object_slicing_hook(PrintObject *print_object, PrintObjectStep object_step, SlicingHookPhase phase);

} // namespace Slic3r
