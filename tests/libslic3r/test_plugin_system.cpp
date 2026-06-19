#include <catch2/catch.hpp>

#include "libslic3r/Plugin/PluginManager.hpp"
#include "libslic3r/Plugin/WipeTowerShapeRegistry.hpp"

using namespace Slic3r;

TEST_CASE("Wipe tower wall type key mapping", "[Plugin][WipeTower]")
{
    PluginManager::instance().initialize();
    register_builtin_wipe_tower_shapes();

    REQUIRE(wipe_tower_wall_type_key(int(wtwRectangle)) == "rectangle");
    REQUIRE(wipe_tower_wall_type_key(int(wtwCone)) == "cone");
    REQUIRE(wipe_tower_wall_type_key(int(wtwRib)) == "rib");
    REQUIRE(wipe_tower_shape_registry().has("rectangle"));
    REQUIRE(wipe_tower_shape_registry().has("cone"));
    REQUIRE(wipe_tower_shape_registry().has("rib"));
}
