#include "InfillProviderRegistryImpl.hpp"

#include "PluginManager.hpp"
#include "libslic3r/Fill/FillBase.hpp"

namespace Slic3r {

void InfillProviderRegistry::register_provider(const std::string &key, Factory factory)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_providers[key] = std::move(factory);
}

Fill *InfillProviderRegistry::create(const std::string &key) const
{
    // Copy the factory out and release the lock before invoking it. The factory
    // may call back into the fill system (and thus this registry) on the same
    // thread; holding m_mutex during the call would self-deadlock (EDEADLK).
    Factory factory;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_providers.find(key);
        if (it == m_providers.end())
            return nullptr;
        factory = it->second;
    }
    return factory ? factory() : nullptr;
}

bool InfillProviderRegistry::has(const std::string &key) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_providers.find(key) != m_providers.end();
}

InfillProviderRegistry &infill_provider_registry()
{
    return PluginManager::instance().infill_providers();
}

void register_infill_provider_defaults()
{
    auto &registry = infill_provider_registry();
    const t_config_enum_values &enum_map = ConfigOptionEnum<InfillPattern>::get_enum_values();
    for (const auto &kv : enum_map) {
        InfillPattern pattern = InfillPattern(kv.second);
        // Use the built-in creator directly. Calling Fill::new_from_type here would
        // route back through this registry and recurse infinitely / self-deadlock.
        registry.register_provider(kv.first, [pattern]() { return Fill::new_from_type_builtin(pattern); });
    }
}

Fill *create_infill_from_type(const InfillPattern type)
{
    const t_config_enum_values &enum_map = ConfigOptionEnum<InfillPattern>::get_enum_values();
    for (const auto &kv : enum_map) {
        if (kv.second == int(type))
            return infill_provider_registry().create(kv.first);
    }
    return nullptr;
}

Fill *create_infill_from_key(const std::string &key)
{
    if (Fill *fill = infill_provider_registry().create(key))
        return fill;
    return Fill::new_from_type(key);
}

} // namespace Slic3r
