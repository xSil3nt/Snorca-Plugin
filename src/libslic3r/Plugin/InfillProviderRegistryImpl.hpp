#ifndef slic3r_InfillProviderRegistryImpl_hpp_
#define slic3r_InfillProviderRegistryImpl_hpp_

#include <orca/plugin/InfillProviderRegistry.hpp>

#include "libslic3r/Fill/FillBase.hpp"

namespace Slic3r {

InfillProviderRegistry &infill_provider_registry();
void register_infill_provider_defaults();
Fill *create_infill_from_type(const InfillPattern type);
Fill *create_infill_from_key(const std::string &key);

} // namespace Slic3r

#endif // slic3r_InfillProviderRegistryImpl_hpp_
