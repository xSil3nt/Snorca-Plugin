#pragma once

#include "AlgorithmProviderRegistry.hpp"

#include <string>

namespace Slic3r {

class PerimeterGenerator;

class IWallGenerator
{
public:
    virtual ~IWallGenerator() = default;
    virtual std::string key() const = 0;
    virtual void        process(PerimeterGenerator &generator) = 0;
};

using WallGeneratorRegistry = AlgorithmProviderRegistry<IWallGenerator>;

} // namespace Slic3r
