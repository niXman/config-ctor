
#ifdef _WIN32
#   include <windows.h>
#endif // _WIN32

#ifdef _MSC_VER
#   include <direct.h>
#   include <process.h>
#else
#   include <unistd.h>
#endif // _MSC_VER

#include <stdlib.h>

#include <string>

namespace config_ctor {
namespace detail {

std::string get_procpath() {
    char buf[1024] = "\0";
#ifdef _WIN32
    ::GetModuleFileNameA(nullptr, buf, sizeof(buf)-1);
#elif defined(__linux__) || defined(__APPLE__)
    if ( ::readlink("/proc/self/exe", buf, sizeof(buf)-1) == -1 )
        return "";
#else
#   error UNKNOWN HOST
#endif
    return buf;
}

std::string get_procname() {
    const std::string procpath = get_procpath();
#ifdef _WIN32
    const std::size_t p = procpath.rfind('\\');
#elif defined(__linux__) || defined(__APPLE__)
    const std::size_t p = procpath.rfind('/');
#else
#   error UNKNOWN HOST
#endif
    return p != std::string::npos ? procpath.substr(p+1) : procpath;
}

} // ns detail
} // ns config_ctor
