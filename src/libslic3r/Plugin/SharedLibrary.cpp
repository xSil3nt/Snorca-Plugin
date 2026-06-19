#include "SharedLibrary.hpp"

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace Slic3r {

SharedLibrary::~SharedLibrary() { unload(); }

SharedLibrary::SharedLibrary(SharedLibrary &&other) noexcept : m_handle(other.m_handle) { other.m_handle = nullptr; }

SharedLibrary &SharedLibrary::operator=(SharedLibrary &&other) noexcept
{
    if (this != &other) {
        unload();
        m_handle       = other.m_handle;
        other.m_handle = nullptr;
    }
    return *this;
}

bool SharedLibrary::load(const std::string &path, std::string &error)
{
    unload();
#ifdef _WIN32
    m_handle = ::LoadLibraryA(path.c_str());
    if (m_handle == nullptr) {
        error = "LoadLibrary failed for " + path;
        return false;
    }
#else
    m_handle = ::dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (m_handle == nullptr) {
        const char *dlerr = ::dlerror();
        error             = std::string("dlopen failed for ") + path + ": " + (dlerr ? dlerr : "unknown error");
        return false;
    }
#endif
    return true;
}

void SharedLibrary::unload()
{
    if (m_handle == nullptr)
        return;
#ifdef _WIN32
    ::FreeLibrary(static_cast<HMODULE>(m_handle));
#else
    ::dlclose(m_handle);
#endif
    m_handle = nullptr;
}

void *SharedLibrary::symbol(const char *name) const
{
    if (m_handle == nullptr || name == nullptr)
        return nullptr;
#ifdef _WIN32
    return reinterpret_cast<void *>(::GetProcAddress(static_cast<HMODULE>(m_handle), name));
#else
    return ::dlsym(m_handle, name);
#endif
}

} // namespace Slic3r
