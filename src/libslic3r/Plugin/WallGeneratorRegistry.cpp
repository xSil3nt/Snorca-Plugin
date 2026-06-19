#include "WallGeneratorRegistry.hpp"

#include "PluginManager.hpp"
#include "libslic3r/PerimeterGenerator.hpp"

namespace Slic3r {

namespace {

class ClassicWallGenerator : public IWallGenerator
{
public:
    std::string key() const override { return "classic"; }
    void process(PerimeterGenerator &generator) override { generator.process_classic(); }
};

class ArachneWallGenerator : public IWallGenerator
{
public:
    std::string key() const override { return "arachne"; }
    void process(PerimeterGenerator &generator) override { generator.process_arachne(); }
};

} // namespace

WallGeneratorRegistry &wall_generator_registry()
{
    return PluginManager::instance().wall_generators();
}

void register_wall_generator_defaults()
{
    auto &registry = wall_generator_registry();
    registry.register_provider("classic", []() { return std::make_unique<ClassicWallGenerator>(); });
    registry.register_provider("arachne", []() { return std::make_unique<ArachneWallGenerator>(); });
}

} // namespace Slic3r
