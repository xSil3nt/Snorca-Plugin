#pragma once

#include "AlgorithmProviderRegistry.hpp"

#include <functional>
#include <map>
#include <mutex>
#include <string>

namespace Slic3r {

class Fill;

class InfillProviderRegistry
{
public:
    using Factory = std::function<Fill *()>;

    void register_provider(const std::string &key, Factory factory);
    Fill *create(const std::string &key) const;
    bool  has(const std::string &key) const;

private:
    mutable std::mutex             m_mutex;
    std::map<std::string, Factory> m_providers;
};

} // namespace Slic3r
