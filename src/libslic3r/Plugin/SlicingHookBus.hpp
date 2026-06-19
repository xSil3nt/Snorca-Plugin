#pragma once
#include <orca/plugin/SlicingHookBus.hpp>

#include "libslic3r/Print.hpp"

namespace Slic3r {

inline SlicingHookContext make_slicing_hook_context(Print *print, PrintObject *print_object, PrintStep print_step)
{
    SlicingHookContext ctx;
    ctx.print       = print;
    ctx.print_object = print_object;
    ctx.print_step  = int(print_step);
    ctx.config      = print ? static_cast<const ConfigBase *>(&print->config()) : nullptr;
    return ctx;
}

inline SlicingHookContext make_slicing_hook_context(PrintObject *print_object, PrintObjectStep object_step)
{
    SlicingHookContext ctx;
    ctx.print        = print_object ? print_object->print() : nullptr;
    ctx.print_object = print_object;
    ctx.object_step  = int(object_step);
    ctx.config       = ctx.print ? static_cast<const ConfigBase *>(&ctx.print->config()) : nullptr;
    return ctx;
}

} // namespace Slic3r
