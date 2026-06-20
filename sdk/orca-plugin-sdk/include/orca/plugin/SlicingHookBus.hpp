#pragma once

#include <functional>
#include <mutex>
#include <string>
#include <vector>

namespace Slic3r {

class Print;
class PrintObject;
class ConfigBase;
class ISliceData;

enum class SlicingHookPhase {
    BeforePrintStep,
    AfterPrintStep,
    BeforePrintObjectStep,
    AfterPrintObjectStep,
    AfterGCodeExport,
};

struct SlicingHookContext
{
    Print            *print{nullptr};
    PrintObject      *print_object{nullptr};
    int               print_step{0};
    int               object_step{0};
    std::string       gcode_path;
    const ConfigBase *config{nullptr};
    ISliceData       *slice_data{nullptr};
};

using SlicingHookFn = std::function<void(const SlicingHookContext &ctx)>;

class SlicingHookBus
{
public:
    void register_hook(SlicingHookPhase phase, SlicingHookFn fn, const std::string &plugin_id = {})
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_hooks.push_back({phase, std::move(fn), plugin_id});
    }

    void fire(SlicingHookPhase phase, const SlicingHookContext &ctx) const;

private:
    struct HookEntry
    {
        SlicingHookPhase phase;
        SlicingHookFn    fn;
        std::string      plugin_id;
    };

    mutable std::mutex     m_mutex;
    std::vector<HookEntry> m_hooks;
};

} // namespace Slic3r
