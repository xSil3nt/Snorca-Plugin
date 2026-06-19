#include <catch2/catch.hpp>

using Catch::Matchers::StartsWith;

#include "libslic3r/Plugin/GCodeTransformRegistry.hpp"
#include "libslic3r/Plugin/PipelineStageRegistry.hpp"
#include "libslic3r/Plugin/PluginManager.hpp"
#include "libslic3r/Plugin/PluginSliceDataAccess.hpp"

using namespace Slic3r;

TEST_CASE("Pipeline stage registry dispatch", "[Plugin][Pipeline]")
{
    auto &registry = PluginManager::instance().pipeline_stages();
    registry.register_handler("infill", "test_pattern", [](PipelineStageContext &ctx) -> bool {
        PluginGeo::ExtrusionPath path;
        path.role = PluginGeo::ExtrusionRole::InternalInfill;
        path.polyline.points.emplace_back(0, 0);
        path.polyline.points.emplace_back(1000, 1000);
        ctx.output.append(std::move(path));
        return true;
    });

    PipelineStageContext ctx;
    ctx.stage_id    = "infill";
    ctx.handler_key = "test_pattern";
    REQUIRE(registry.run("infill", "test_pattern", ctx));
    REQUIRE(ctx.output.paths.size() == 1);
    REQUIRE(registry.has("infill", "test_pattern"));
}

TEST_CASE("G-code transform registry rewrites layer output", "[Plugin][GCodeTransform]")
{
    auto &registry = PluginManager::instance().gcode_transforms();
    registry.register_layer_transform("test_marker", [](LayerGCodeTransformContext &ctx) {
        ctx.gcode.insert(0, "; plugin_marker\n");
    });

    LayerGCodeTransformContext ctx;
    ctx.gcode    = "G1 X1 Y1\n";
    ctx.layer_id = 3;
    registry.apply_layer_transforms(ctx);
    REQUIRE_THAT(ctx.gcode, Catch::Matchers::StartsWith("; plugin_marker"));
}

TEST_CASE("SDK extrusion role round-trip", "[Plugin][Geometry]")
{
    REQUIRE(to_host_extrusion_role(PluginGeo::ExtrusionRole::OverhangPerimeter) == erOverhangPerimeter);
    REQUIRE(to_sdk_extrusion_role(erInternalInfill) == PluginGeo::ExtrusionRole::InternalInfill);
}
