
#include <config-ctor/config-ctor.hpp>

#include <iostream>

#define MY_ASSERT(...) \
    if ( !(__VA_ARGS__) ) { \
        std::cerr << "expression error(" __FILE__ ":" BOOST_PP_STRINGIZE(__LINE__) "): \"" #__VA_ARGS__ "\"" << std::endl; \
        std::abort(); \
    }

#define DUMP_CONFIG(os, ss, descr) \
    os << "[" descr "]:" << std::endl; \
    os << ss.str() << std::endl;

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

    return EXIT_SUCCESS;
}

/***************************************************************************/
