
#include <config-ctor/config-ctor.hpp>

#include <iostream>

#ifdef _WIN32
#   include <windows.h>
#endif // _WIN32

/***************************************************************************/

#define MY_ASSERT(...) \
    if ( !(__VA_ARGS__) ) { \
        std::cerr << "expression error(" __FILE__ ":" BOOST_PP_STRINGIZE(__LINE__) "): \"" #__VA_ARGS__ "\"" << std::endl; \
        std::abort(); \
    }

#define DUMP_CONFIG(os, ss, descr) \
    os << "[" descr "]:" << std::endl; \
    os << ss.str() << std::endl;

#define MY_ENV_TEST(env, exp) { \
    CONSTRUCT_INI_CONFIG( \
        config, \
        (std::string, path) \
    ) \
    std::stringstream ss; \
    ss << "path=" << env << std::endl; \
    config cfg = config::read(ss); \
    MY_ASSERT(cfg.path == exp) ; \
}

/***************************************************************************/

std::string get_user() {
#ifdef _WIN32
    return ::getenv("USERNAME");
#elif defined(__linux__) || defined(__APPLE__)
    return ::getenv("USER");
#endif // _WIN32
}

std::string get_home() {
#ifdef _WIN32
    return ::getenv("USERPROFILE");
#elif defined(__linux__) || defined(__APPLE__)
    return ::getenv("HOME");
#endif // _WIN32
}

std::string get_temp() {
    if (const char *temp = ::getenv("TMPDIR")) {
        return temp;
    } else if (const char *temp = ::getenv("TEMP")) {
        return temp;
    } else if (const char *temp = ::getenv("TMP")) {
        return temp;
    }
    return "/tmp";
}

std::string get_cwd() { return ::getcwd(nullptr, 1024); }
std::string get_pid() { return std::to_string(::getpid()); }

std::string get_proc_path() {
    char buf[1024] = "\0";
#ifdef _WIN32
    ::GetModuleFileName(nullptr, buf, sizeof(buf)-1);
#elif defined(__linux__) || defined(__APPLE__)
    ::readlink("/proc/self/exe", buf, sizeof(buf)-1);
#else
#   error UNKNOWN HOST
#endif

    return buf;
}

std::string get_proc_name() {
    const std::string procpath = get_proc_path();
#ifdef _WIN32
    const std::size_t p = procpath.rfind('\\');
#elif defined(__linux__) || defined(__APPLE__)
    const std::size_t p = procpath.rfind('/');
#else
#   error UNKNOWN HOST
#endif

    return p != std::string::npos ? procpath.substr(p+1) : procpath;
}

/***************************************************************************/

int main() {
    //////////////////////////////////////////////// ini
    {
        CONSTRUCT_INI_CONFIG(
            config,
            (int, a)
            (float, b)
            (std::string, c)
        )

        static const int a = 33;
        static const float b = 44.55;
        static const std::string c = "some string";

        config wcfg;
        wcfg.a = a;
        wcfg.b = b;
        wcfg.c = c;

        std::stringstream ss;
        config::write(ss, wcfg);
        DUMP_CONFIG(std::cout, ss, "test 'ini' config dump")

        const config rcfg = config::read(ss);
        MY_ASSERT(rcfg.a == a);
        MY_ASSERT(rcfg.b == b);
        MY_ASSERT(rcfg.c == c);
    }
    //////////////////////////////////////////////// json
    {
        CONSTRUCT_JSON_CONFIG(
            config,
            (double, a)
            (long, b)
            (std::uint16_t, c)
        )

        static const double a = 33.44;
        static const long b = 55;
        static const std::uint16_t c = 66;

        config wcfg;
        wcfg.a = a;
        wcfg.b = b;
        wcfg.c = c;

        std::stringstream ss;
        config::write(ss, wcfg);
        DUMP_CONFIG(std::cout, ss, "test 'json' config dump")

        const config rcfg = config::read(ss);
        MY_ASSERT(rcfg.a == a);
        MY_ASSERT(rcfg.b == b);
        MY_ASSERT(rcfg.c == c);
    }
    //////////////////////////////////////////////// xml
    {
        CONSTRUCT_XML_CONFIG(
            config,
            (double, a)
            (long, b)
            (std::uint16_t, c)
        )

        static const double a = 33.44;
        static const long b = 55;
        static const std::uint16_t c = 66;

        config wcfg;
        wcfg.a = a;
        wcfg.b = b;
        wcfg.c = c;

        std::stringstream ss;
        config::write(ss, wcfg);
        DUMP_CONFIG(std::cout, ss, "test 'xml' config dump")

        const config rcfg = config::read(ss);
        MY_ASSERT(rcfg.a == a);
        MY_ASSERT(rcfg.b == b);
        MY_ASSERT(rcfg.c == c);
    }
    //////////////////////////////////////////////// info
    {
        CONSTRUCT_INFO_CONFIG(
            config,
            (double, a)
            (long, b)
            (std::uint16_t, c)
        )

        static const double a = 33.44;
        static const long b = 55;
        static const std::uint16_t c = 66;

        config wcfg;
        wcfg.a = a;
        wcfg.b = b;
        wcfg.c = c;

        std::stringstream ss;
        config::write(ss, wcfg);
        DUMP_CONFIG(std::cout, ss, "test 'info' config dump")

        const config rcfg = config::read(ss);
        MY_ASSERT(rcfg.a == a);
        MY_ASSERT(rcfg.b == b);
        MY_ASSERT(rcfg.c == c);
    }
    //////////////////////////////////////////////// default value test
    {
        CONSTRUCT_INI_CONFIG(
            config,
            (int, a)
            (bool, b, false)
            (float, c, 22)
            (std::string, d)
            (std::string, e, /path/to/some/file.ext)
            (long, f)
        )

        static const int a = 33;
        static const std::string d = "string";

        std::stringstream ss;
        ss << "a=" << a << std::endl
           << "d=" << d << std::endl
           << "f=64k" << std::endl;

        const config rcfg = config::read(ss);
        MY_ASSERT(rcfg.a == a);
        MY_ASSERT(rcfg.b == false);
        MY_ASSERT(rcfg.c == 22);
        MY_ASSERT(rcfg.d == d);
        MY_ASSERT(rcfg.e == "/path/to/some/file.ext");
        MY_ASSERT(rcfg.f == 64*1024);
    }
    //////////////////////////////////////////////// env test
    {
        const std::string user = get_user(); MY_ASSERT(user.size());
        std::cout << "user=" << user << std::endl;
        const std::string home = get_home(); MY_ASSERT(home.size());
        std::cout << "home=" << home << std::endl;
        const std::string path = getenv("PATH") ; MY_ASSERT(path.size());
        std::cout << "path=" << path << std::endl;
        const std::string cwd  = get_cwd()      ; MY_ASSERT(cwd.size());
        std::cout << "cwd=" << cwd << std::endl;
        const std::string temp = get_temp()     ; MY_ASSERT(temp.size());
        std::cout << "temp=" << temp << std::endl;
        const std::string pid  = get_pid()      ; MY_ASSERT(pid.size());
        std::cout << "pid=" << pid << std::endl;
        const std::string procp = get_proc_path(); MY_ASSERT(procp.size());
        std::cout << "procp=" << procp << std::endl;
        const std::string proc = get_proc_name(); MY_ASSERT(proc.size());
        std::cout << "proc=" << proc << std::endl;

        // USER
        MY_ENV_TEST("{USER}", user);
        MY_ENV_TEST("/{USER}", "/"+user);
        MY_ENV_TEST("{USER}/", user+"/");
        MY_ENV_TEST("/{USER}/", "/"+user+"/");
        MY_ENV_TEST("/1/{USER}/2", "/1/"+user+"/2");

        // HOME
        MY_ENV_TEST("{HOME}", home);
        MY_ENV_TEST("/{HOME}", "/"+home);
        MY_ENV_TEST("{HOME}/", home+"/");
        MY_ENV_TEST("/{HOME}/", "/"+home+"/");
        MY_ENV_TEST("/1/{HOME}/2", "/1/"+home+"/2");

        // TEMP
        MY_ENV_TEST("{TEMP}", temp);
        MY_ENV_TEST("/{TEMP}", "/"+temp);
        MY_ENV_TEST("{TEMP}/", temp+"/");
        MY_ENV_TEST("/{TEMP}/", "/"+temp+"/");
        MY_ENV_TEST("/1/{TEMP}/2", "/1/"+temp+"/2");

        // CWD
        MY_ENV_TEST("{CWD}", cwd);
        MY_ENV_TEST("/{CWD}", "/"+cwd);
        MY_ENV_TEST("{CWD}/", cwd+"/");
        MY_ENV_TEST("/{CWD}/", "/"+cwd+"/");
        MY_ENV_TEST("/1/{CWD}/2", "/1/"+cwd+"/2");

        // PID
        MY_ENV_TEST("{PID}", pid);
        MY_ENV_TEST("/{PID}", "/"+pid);
        MY_ENV_TEST("{PID}/", pid+"/");
        MY_ENV_TEST("/{PID}/", "/"+pid+"/");
        MY_ENV_TEST("/1/{PID}/2", "/1/"+pid+"/2");

        // PATH
        MY_ENV_TEST("{PATH}", path);
        MY_ENV_TEST("/{PATH}", "/"+path);
        MY_ENV_TEST("{PATH}/", path+"/");
        MY_ENV_TEST("/{PATH}/", "/"+path+"/");
        MY_ENV_TEST("/1/{PATH}/2", "/1/"+path+"/2");

        // PROC
        MY_ENV_TEST("{PROC}", proc);
        MY_ENV_TEST("/{PROC}", "/"+proc);
        MY_ENV_TEST("{PROC}/", proc+"/");
        MY_ENV_TEST("/{PROC}/", "/"+proc+"/");
        MY_ENV_TEST("/1/{PROC}/2", "/1/"+proc+"/2");

        // PROCPATH
        MY_ENV_TEST("{PROCPATH}", procp);
        MY_ENV_TEST("/{PROCPATH}", "/"+procp);
        MY_ENV_TEST("{PROCPATH}/", procp+"/");
        MY_ENV_TEST("/{PROCPATH}/", "/"+procp+"/");
        MY_ENV_TEST("/1/{PROCPATH}/2", "/1/"+procp+"/2");
    }

    return EXIT_SUCCESS;
}

/***************************************************************************/
