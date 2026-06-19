namespace {

class WipeTowerPathWriterAdapter : public IWipeTowerPathWriter
{
public:
    explicit WipeTowerPathWriterAdapter(WipeTowerWriter2 &writer) : m_writer(writer) {}

    IWipeTowerPathWriter &travel(float x, float y, float feedrate = 0.f) override
    {
        m_writer.travel(x, y, feedrate);
        return *this;
    }
    IWipeTowerPathWriter &travel(const PluginGeo::Vec2f &dest, float feedrate = 0.f) override
    {
        m_writer.travel(dest.x(), dest.y(), feedrate);
        return *this;
    }
    IWipeTowerPathWriter &extrude(float x, float y, float feedrate = 0.f) override
    {
        m_writer.extrude(x, y, feedrate);
        return *this;
    }
    IWipeTowerPathWriter &extrude(const PluginGeo::Vec2f &dest, float feedrate = 0.f) override
    {
        m_writer.extrude(dest.x(), dest.y(), feedrate);
        return *this;
    }
    IWipeTowerPathWriter &extrude_explicit(float x, float y, float e, float feedrate = 0.f) override
    {
        m_writer.extrude_explicit(x, y, e, feedrate);
        return *this;
    }
    IWipeTowerPathWriter &extrude_explicit(const PluginGeo::Vec2f &dest, float e, float feedrate = 0.f) override
    {
        m_writer.extrude_explicit(dest.x(), dest.y(), e, feedrate);
        return *this;
    }
    IWipeTowerPathWriter &ram(float x1, float x2, float dy, float e0, float e, float feedrate) override
    {
        m_writer.ram(x1, x2, dy, e0, e, feedrate);
        return *this;
    }
    IWipeTowerPathWriter &retract(float e, float feedrate = 0.f) override
    {
        m_writer.retract(e, feedrate);
        return *this;
    }
    IWipeTowerPathWriter &load(float e, float feedrate = 0.f) override
    {
        m_writer.load(e, feedrate);
        return *this;
    }

    float x() const override { return m_writer.x(); }
    float y() const override { return m_writer.y(); }
    const PluginGeo::Vec2f &pos() const override
    {
        m_pos.x() = m_writer.x();
        m_pos.y() = m_writer.y();
        return m_pos;
    }
    IWipeTowerPathWriter &feedrate(float feedrate) override
    {
        m_writer.feedrate(feedrate);
        return *this;
    }

    IWipeTowerPathWriter &append(const std::string &text) override
    {
        m_writer.append(text);
        return *this;
    }
    IWipeTowerPathWriter &comment_with_value(const char *comment, int value) override
    {
        m_writer.comment_with_value(comment, value);
        return *this;
    }

    IWipeTowerPathWriter &rectangle(const PluginGeo::Vec2f &ld, float width, float height, float feedrate = 0.f) override
    {
        m_writer.rectangle(Vec2f(ld.x(), ld.y()), width, height, feedrate);
        return *this;
    }
    IWipeTowerPathWriter &suppress_preview() override
    {
        m_writer.suppress_preview();
        return *this;
    }
    IWipeTowerPathWriter &resume_preview() override
    {
        m_writer.resume_preview();
        return *this;
    }
    IWipeTowerPathWriter &change_analyzer_line_width(float line_width) override
    {
        m_writer.change_analyzer_line_width(line_width);
        return *this;
    }
    IWipeTowerPathWriter &set_extrusion_flow(float flow) override
    {
        m_writer.set_extrusion_flow(flow);
        return *this;
    }
    IWipeTowerPathWriter &add_wipe_point(float x, float y) override
    {
        m_writer.add_wipe_point(x, y);
        return *this;
    }
    IWipeTowerPathWriter &add_wipe_point(const PluginGeo::Vec2f &pt) override
    {
        m_writer.add_wipe_point(pt.x(), pt.y());
        return *this;
    }

    IWipeTowerPathWriter &set_extruder_temp(int temperature, bool wait = false) override
    {
        m_writer.set_extruder_temp(temperature, wait);
        return *this;
    }
    IWipeTowerPathWriter &disable_linear_advance() override
    {
        m_writer.disable_linear_advance();
        return *this;
    }
    void spiral_flat_ironing(const PluginGeo::Vec2f &center, float area, float step_length, float feedrate) override
    {
        m_writer.spiral_flat_ironing(Vec2f(center.x(), center.y()), area, step_length, feedrate);
    }

private:
    WipeTowerWriter2    &m_writer;
    mutable PluginGeo::Vec2f m_pos;

public:
    void sync_pos() { m_pos = PluginGeo::Vec2f(m_writer.x(), m_writer.y()); }
};

inline WipeTowerHostServices make_wipe_tower_host_services()
{
    WipeTowerHostServices services;
    services.volume_to_length = volume_to_length;
    services.length_to_volume = length_to_volume;
    services.get_wipe_depth   = get_wipe_depth;
    return services;
}

inline float wipe_channel_width_for_shape(const std::string &wall_shape_key, const WipeTowerPlanningContext &planning)
{
    if (auto shape = wipe_tower_shape_registry().create(wall_shape_key)) {
        const float plugin_width = shape->get_wipe_channel_width(planning);
        if (plugin_width > 0.f)
            return plugin_width;
    }
    return planning.wipe_tower_width - 3.f * planning.perimeter_width;
}

} // namespace
