
#include <config-ctor/config-ctor.hpp>

#include <iostream>

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

std::string get_temp() {
#ifdef _WIN32
    char buf[MAX_PATH+1+1];
    ::GetTempPath(sizeof(buf), buf);
    return buf;
#elif defined(__linux__)
    if (const char *temp = ::getenv("TMPDIR")) {
        return temp;
    } else if (const char *temp = ::getenv("TEMP")) {
        return temp;
    } else if (const char *temp = ::getenv("TMP")) {
        return temp;
    }
    return "/tmp";
#else
#   error UNKNOWN HOST
#endif
}

std::string get_cwd() { return ::getcwd(nullptr, 1024); }
std::string get_pid() { return std::to_string(::getpid()); }

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
        const std::string user = ::getenv("USER"); MY_ASSERT(user.size());
        const std::string home = ::getenv("HOME"); MY_ASSERT(home.size());
        const std::string path = ::getenv("PATH"); MY_ASSERT(path.size());
        const std::string cwd  = ::get_cwd()     ; MY_ASSERT(cwd.size());
        const std::string temp = ::get_temp()    ; MY_ASSERT(temp.size());
        const std::string pid  = ::get_pid()     ; MY_ASSERT(pid.size());

        // USER
        MY_ENV_TEST("{USER}", user);
        MY_ENV_TEST("/{USER}", std::string("/") + user);
        MY_ENV_TEST("{USER}/", user + std::string("/"));
        MY_ENV_TEST("/{USER}/", std::string("/") + user + std::string("/"));
        MY_ENV_TEST("/1/{USER}/2", std::string("/1/") + user + std::string("/2"));

        // HOME
        MY_ENV_TEST("{HOME}", home);
        MY_ENV_TEST("/{HOME}", std::string("/") + home);
        MY_ENV_TEST("{HOME}/", home + std::string("/"));
        MY_ENV_TEST("/{HOME}/", std::string("/") + home + std::string("/"));
        MY_ENV_TEST("/1/{HOME}/2", std::string("/1/") + home + std::string("/2"));

        // TEMP
        MY_ENV_TEST("{TEMP}", temp);
        MY_ENV_TEST("/{TEMP}", std::string("/") + temp);
        MY_ENV_TEST("{TEMP}/", temp + std::string("/"));
        MY_ENV_TEST("/{TEMP}/", std::string("/") + temp + std::string("/"));
        MY_ENV_TEST("/1/{TEMP}/2", std::string("/1/") + temp + std::string("/2"));

        // CWD
        MY_ENV_TEST("{CWD}", cwd);
        MY_ENV_TEST("/{CWD}", std::string("/") + cwd);
        MY_ENV_TEST("{CWD}/", cwd + std::string("/"));
        MY_ENV_TEST("/{CWD}/", std::string("/") + cwd + std::string("/"));
        MY_ENV_TEST("/1/{CWD}/2", std::string("/1/") + cwd + std::string("/2"));

        // PID
        MY_ENV_TEST("{PID}", pid);
        MY_ENV_TEST("/{PID}", std::string("/") + pid);
        MY_ENV_TEST("{PID}/", pid + std::string("/"));
        MY_ENV_TEST("/{PID}/", std::string("/") + pid + std::string("/"));
        MY_ENV_TEST("/1/{PID}/2", std::string("/1/") + pid + std::string("/2"));

        // PATH
        MY_ENV_TEST("{PATH}", path);
        MY_ENV_TEST("/{PATH}", std::string("/") + path);
        MY_ENV_TEST("{PATH}/", path + std::string("/"));
        MY_ENV_TEST("/{PATH}/", std::string("/") + path + std::string("/"));
        MY_ENV_TEST("/1/{PATH}/2", std::string("/1/") + path + std::string("/2"));
    }

    return EXIT_SUCCESS;
}

/***************************************************************************/
