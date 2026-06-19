#pragma once

#include "geometry/ExtrusionTypes.hpp"
#include "geometry/Polygon.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace Slic3r {

class IPluginConfigReader
{
public:
    virtual ~IPluginConfigReader() = default;

    virtual bool        has(const std::string &key) const = 0;
    virtual std::string get_string(const std::string &key) const = 0;
    virtual double      get_float(const std::string &key) const = 0;
    virtual int         get_int(const std::string &key) const = 0;
};

enum class PluginSliceCollectionKind {
    Perimeters,
    Fills,
    Support,
    ThinFills,
};

class ISliceData
{
public:
    virtual ~ISliceData() = default;

    virtual IPluginConfigReader *config() = 0;
    virtual size_t               layer_count() const = 0;
    virtual size_t               region_count() const = 0;

    virtual bool get_extrusion_collection(size_t layer_id,
                                          size_t region_id,
                                          PluginSliceCollectionKind kind,
                                          PluginGeo::ExtrusionCollection &out) const = 0;

    virtual bool set_extrusion_collection(size_t layer_id,
                                          size_t region_id,
                                          PluginSliceCollectionKind kind,
                                          const PluginGeo::ExtrusionCollection &in) = 0;

    virtual std::vector<PluginGeo::Polygon> get_region_slices(size_t layer_id, size_t region_id) const = 0;
};

} // namespace Slic3r
