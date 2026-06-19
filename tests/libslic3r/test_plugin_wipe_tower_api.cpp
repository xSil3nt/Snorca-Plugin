#include <catch2/catch.hpp>

#include <orca/plugin/IWipeTowerPathWriter.hpp>
#include <orca/plugin/IWipeTowerShape.hpp>
#include <orca/plugin/WipeTowerTypes.hpp>

using namespace Slic3r;

namespace {

class MockPathWriter : public IWipeTowerPathWriter
{
public:
    bool scaffold_called{false};

    IWipeTowerPathWriter &travel(float, float, float = 0.f) override { return *this; }
    IWipeTowerPathWriter &travel(const PluginGeo::Vec2f &, float = 0.f) override { return *this; }
    IWipeTowerPathWriter &extrude(float, float, float = 0.f) override { return *this; }
    IWipeTowerPathWriter &extrude(const PluginGeo::Vec2f &, float = 0.f) override { return *this; }
    IWipeTowerPathWriter &extrude_explicit(float, float, float, float = 0.f) override { return *this; }
    IWipeTowerPathWriter &extrude_explicit(const PluginGeo::Vec2f &, float, float = 0.f) override { return *this; }
    IWipeTowerPathWriter &ram(float, float, float, float, float, float) override { return *this; }
    IWipeTowerPathWriter &retract(float, float = 0.f) override { return *this; }
    IWipeTowerPathWriter &load(float, float = 0.f) override { return *this; }
    float x() const override { return 0.f; }
    float y() const override { return 0.f; }
    const PluginGeo::Vec2f &pos() const override { return m_pos; }
    IWipeTowerPathWriter &feedrate(float) override { return *this; }
    IWipeTowerPathWriter &append(const std::string &) override { return *this; }
    IWipeTowerPathWriter &comment_with_value(const char *, int) override { return *this; }
    IWipeTowerPathWriter &rectangle(const PluginGeo::Vec2f &, float, float, float = 0.f) override { return *this; }
    IWipeTowerPathWriter &suppress_preview() override { return *this; }
    IWipeTowerPathWriter &resume_preview() override { return *this; }
    IWipeTowerPathWriter &change_analyzer_line_width(float) override { return *this; }
    IWipeTowerPathWriter &set_extrusion_flow(float) override { return *this; }
    IWipeTowerPathWriter &add_wipe_point(float, float) override { return *this; }
    IWipeTowerPathWriter &add_wipe_point(const PluginGeo::Vec2f &) override { return *this; }
    IWipeTowerPathWriter &set_extruder_temp(int, bool = false) override { return *this; }
    IWipeTowerPathWriter &disable_linear_advance() override { return *this; }
    void spiral_flat_ironing(const PluginGeo::Vec2f &, float, float, float) override {}

private:
    PluginGeo::Vec2f m_pos;
};

class MockScaffoldShape : public IWipeTowerShape
{
public:
    std::string key() const override { return "mock_scaffold"; }
    PluginGeo::Polygon generate_wall(const WipeTowerWallContext &) override { return {}; }
    bool generate_sparse_scaffold(const WipeTowerScaffoldContext &, IWipeTowerPathWriter &writer) override
    {
        writer.append("; mock scaffold");
        return true;
    }
};

} // namespace

TEST_CASE("Wipe tower scaffold hook skips host default", "[Plugin][WipeTower]")
{
    MockScaffoldShape shape;
    MockPathWriter    writer;
    WipeTowerScaffoldContext ctx;
    ctx.layer.perimeter_width = 0.4f;
    ctx.fill_box              = WipeTowerBoxCoordinates(0.f, 0.f, 20.f, 10.f);

    REQUIRE(shape.generate_sparse_scaffold(ctx, writer));
}

TEST_CASE("Wipe tower default hooks return false", "[Plugin][WipeTower]")
{
    MockScaffoldShape shape;
    MockPathWriter    writer;
    WipeTowerScaffoldContext  scaffold_ctx;
    WipeTowerToolchangeContext toolchange_ctx;

    REQUIRE_FALSE(shape.generate_toolchange_wipe(toolchange_ctx, writer));
    REQUIRE(shape.get_wipe_channel_width({}) == 0.f);
}
