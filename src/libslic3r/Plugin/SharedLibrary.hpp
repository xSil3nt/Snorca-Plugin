#ifndef slic3r_SharedLibrary_hpp_
#define slic3r_SharedLibrary_hpp_

#include <string>

namespace Slic3r {

class SharedLibrary
{
public:
    SharedLibrary() = default;
    ~SharedLibrary();

    SharedLibrary(const SharedLibrary &)            = delete;
    SharedLibrary &operator=(const SharedLibrary &) = delete;
    SharedLibrary(SharedLibrary &&other) noexcept;
    SharedLibrary &operator=(SharedLibrary &&other) noexcept;

    bool load(const std::string &path, std::string &error);
    void unload();

    void *symbol(const char *name) const;
    bool  loaded() const { return m_handle != nullptr; }

private:
    void *m_handle{nullptr};
};

} // namespace Slic3r

#endif // slic3r_SharedLibrary_hpp_
