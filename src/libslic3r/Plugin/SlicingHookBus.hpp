#ifndef slic3r_SlicingHookBus_hpp_
#define slic3r_SlicingHookBus_hpp_

#include "libslic3r/Config.hpp"

#include <functional>
#include <mutex>
#include <string>
#include <vector>

namespace Slic3r {

class Print;
class PrintObject;
enum PrintStep : int;
enum PrintObjectStep : int;

enum class SlicingHookPhase {
    BeforePrintStep,
    AfterPrintStep,
    BeforePrintObjectStep,
    AfterPrintObjectStep,
    AfterGCodeExport,
};

struct SlicingHookContext
{
    Print                              *print{nullptr};
    PrintObject                        *print_object{nullptr};
    PrintStep                           print_step;
    PrintObjectStep                     object_step;
    std::string                         gcode_path;
    const ConfigBase                   *config{nullptr};
};

using SlicingHookFn = std::function<void(const SlicingHookContext &ctx)>;

class SlicingHookBus
{
public:
    void register_hook(SlicingHookPhase phase, SlicingHookFn fn, const std::string &plugin_id = {});

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

#endif // slic3r_SlicingHookBus_hpp_
