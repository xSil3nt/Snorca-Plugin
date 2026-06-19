#ifndef slic3r_InfillProviderRegistryImpl_hpp_
#define slic3r_InfillProviderRegistryImpl_hpp_

#include "libslic3r/Fill/FillBase.hpp"

#include <functional>
#include <map>
#include <mutex>
#include <string>

namespace Slic3r {

class InfillProviderRegistry
{
public:
    using Factory = std::function<Fill *()>;

    void register_provider(const std::string &key, Factory factory);
    Fill          *create(const std::string &key) const;
    bool           has(const std::string &key) const;

private:
    mutable std::mutex              m_mutex;
    std::map<std::string, Factory>  m_providers;
};

InfillProviderRegistry &infill_provider_registry();
void register_infill_provider_defaults();
Fill *create_infill_from_type(const InfillPattern type);
Fill *create_infill_from_key(const std::string &key);

} // namespace Slic3r

#endif // slic3r_InfillProviderRegistryImpl_hpp_
