#include "SlicingHookBus.hpp"

namespace Slic3r {

void SlicingHookBus::register_hook(SlicingHookPhase phase, SlicingHookFn fn, const std::string &plugin_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_hooks.push_back({phase, std::move(fn), plugin_id});
}

void SlicingHookBus::fire(SlicingHookPhase phase, const SlicingHookContext &ctx) const
{
    std::vector<SlicingHookFn> hooks;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        hooks.reserve(m_hooks.size());
        for (const HookEntry &entry : m_hooks) {
            if (entry.phase == phase && entry.fn)
                hooks.push_back(entry.fn);
        }
    }
    for (const SlicingHookFn &fn : hooks)
        fn(ctx);
}

} // namespace Slic3r
