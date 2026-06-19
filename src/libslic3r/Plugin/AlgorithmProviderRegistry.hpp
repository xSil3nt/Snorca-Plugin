#ifndef slic3r_AlgorithmProviderRegistry_hpp_
#define slic3r_AlgorithmProviderRegistry_hpp_

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace Slic3r {

template<typename Interface>
class AlgorithmProviderRegistry
{
public:
    using Factory = std::function<std::unique_ptr<Interface>()>;

    void register_provider(const std::string &key, Factory factory)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_providers[key] = std::move(factory);
    }

    bool has(const std::string &key) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_providers.find(key) != m_providers.end();
    }

    std::unique_ptr<Interface> create(const std::string &key) const
    {
        // Copy the factory out and release the lock before invoking it, so a
        // factory that calls back into this registry on the same thread cannot
        // self-deadlock (EDEADLK) on m_mutex.
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

    std::vector<std::string> keys() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<std::string> out;
        out.reserve(m_providers.size());
        for (const auto &kv : m_providers)
            out.push_back(kv.first);
        return out;
    }

private:
    mutable std::mutex              m_mutex;
    std::map<std::string, Factory>  m_providers;
};

} // namespace Slic3r

#endif // slic3r_AlgorithmProviderRegistry_hpp_
