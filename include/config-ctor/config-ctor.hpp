
// Copyright (c) 2014-2017 niXman (i dot nixman dog gmail dot com). All
// rights reserved.
//
// This file is part of CONFIG-CTOR(https://github.com/niXman/config-ctor) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef __config_ctor__config_ctor_hpp
#define __config_ctor__config_ctor_hpp

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/comparison/greater_equal.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/tuple/size.hpp>
#include <boost/preprocessor/tuple/elem.hpp>

#include <cstdint>
#include <string>
#include <iosfwd>
#include <map>

#ifdef _WIN32
#   include <direct.h>
#else
#   include <unistd.h>
#endif // _WIN32

#include <stdlib.h>

/***************************************************************************/

namespace config_ctor {

namespace detail {
std::string get_procpath();
std::string get_procname();
} // ns detail

namespace {

/***************************************************************************/

// for other types (strings)
template<bool ok>
struct get_concrete_value {
    template<typename T>
    static T get(const char *key, const boost::property_tree::ptree &ini, const char *default_value) {
        std::string res = default_value != nullptr
           ? ini.get<T>(key, default_value)
           : ini.get<T>(key)
        ;

        static auto get_home = []() -> std::string {
        #ifdef _WIN32
            return ::getenv("USERPROFILE");
        #elif defined(__linux__) || defined(__APPLE__)
            return ::getenv("HOME");
        #else
        #   error UNKNOWN HOST
        #endif
        };
        static auto get_user = []() -> std::string {
        #ifdef _WIN32
            return ::getenv("USERNAME");
        #elif defined(__linux__) || defined(__APPLE__)
            return ::getenv("USER");
        #else
        #   error UNKNOWN HOST
        #endif
        };
        static auto get_cwd  = []() -> std::string {
            char buf[1024];
            return ::getcwd(buf, sizeof(buf));
        };
        static auto get_temp = []() -> std::string {
            if (const char *temp = ::getenv("TMPDIR")) {
                return temp;
            } else if (const char *temp = ::getenv("TEMP")) {
                return temp;
            } else if (const char *temp = ::getenv("TMP")) {
                return temp;
            }
            return "/tmp";
        };
        static auto get_pid  = []() -> std::string {
            int pid = ::getpid();
            return std::to_string(pid);
        };
        static auto get_path = []() -> std::string { return ::getenv("PATH"); };
        static auto get_proc_name = []() -> std::string { return detail::get_procname(); };
        static auto get_proc_path = []() -> std::string { return detail::get_procpath(); };

        static const std::map<std::string, std::function<std::string()>> map = {
             {"{HOME}", std::move(get_home)}
            ,{"{USER}", std::move(get_user)}
            ,{"{CWD}" , std::move(get_cwd )}
            ,{"{TEMP}", std::move(get_temp)}
            ,{"{PID}" , std::move(get_pid )}
            ,{"{PATH}", std::move(get_path)}
            ,{"{PROC}", std::move(get_proc_name)}
            ,{"{PROCPATH}", std::move(get_proc_path)}
        };

        static auto replace = [](std::string &str, const std::string &ostr, const std::string &nstr) {
            std::string::size_type pos = 0u;
            while ( (pos = str.find(ostr, pos)) != std::string::npos ) {
                str.replace(pos, ostr.length(), nstr);
                pos += nstr.length();
            }
        };

        for (const auto &it: map) {
            replace(res, it.first, it.second());
        }

        return res;
    }
};

// handle other arithmetic types
template<typename T>
struct bool_case {
    static T apply(const char *key, const boost::property_tree::ptree &ini, const char *default_value) {
        std::string val = (default_value != nullptr)
            ? ini.get<std::string>(key, default_value)
            : ini.get<std::string>(key)
        ;

        if ( val.empty() ) return T{};

        if ( std::is_integral<T>::value ) {
            std::int64_t mult = 1;
            switch ( val.back() ) {
                case 't': case 'T': mult *= 1024; // fallthrough
                case 'g': case 'G': mult *= 1024; // fallthrough
                case 'm': case 'M': mult *= 1024; // fallthrough
                case 'k': case 'K': mult *= 1024; // fallthrough

                    val.pop_back();
            }

            if ( std::is_unsigned<T>::value ) {
                return static_cast<T>(std::stoul(val) * mult);
            } else {
                return static_cast<T>(std::stol(val) * mult);
            }
        }

        return static_cast<T>(std::stod(val));
    }
};

// handle bool types
template<>
struct bool_case<bool> {
    static bool apply(const char *key, const boost::property_tree::ptree &ini, const char *default_value) {
        return (default_value != nullptr)
            ? ini.get<bool>(key, (std::strcmp(default_value, "true") == 0))
            : ini.get<bool>(key)
        ;
    }
};

// specialization for arithmetic types
template<>
struct get_concrete_value<true> {
    template<typename T>
    static T get(const char *key, const boost::property_tree::ptree &ini, const char *default_value) {
        return bool_case<T>::apply(key, ini, default_value);
    }
};

} // anon ns

template<typename T>
static T get_value(const char *key, const boost::property_tree::ptree &cfg, const char *default_value) {
    using TT = typename std::remove_cv<T>::type;
    return get_concrete_value<std::is_arithmetic<TT>::value>::template get<TT>(key, cfg, default_value);
}

template<bool ok>
struct print_value {
    template<typename T>
    static void print(const T &v, std::ostream &os) {
        os << '\"' << v << '\"';
    }
};

template<>
struct print_value<true> {
    template<typename T>
    static void print(const T &v, std::ostream &os) {
        os << std::boolalpha << v;
    }
};

} // ns config_ctor

/***************************************************************************/

#define _CONSTRUCT_CONFIG__WRAP_SEQUENCE_X(...) \
    ((__VA_ARGS__)) _CONSTRUCT_CONFIG__WRAP_SEQUENCE_Y
#define _CONSTRUCT_CONFIG__WRAP_SEQUENCE_Y(...) \
    ((__VA_ARGS__)) _CONSTRUCT_CONFIG__WRAP_SEQUENCE_X

#define _CONSTRUCT_CONFIG__WRAP_SEQUENCE_X0
#define _CONSTRUCT_CONFIG__WRAP_SEQUENCE_Y0

/***************************************************************************/

#define _CONSTRUCT_CONFIG__GENERATE_MEMBERS(unused, data, idx, elem) \
    BOOST_PP_TUPLE_ELEM(0, elem) /* type */ BOOST_PP_TUPLE_ELEM(1, elem) /* var name */ ;

#define _CONSTRUCT_CONFIG__INIT_MEMBERS_WITH_DEFAULT(...) \
    ,BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, __VA_ARGS__))

#define _CONSTRUCT_CONFIG__INIT_MEMBERS_WITHOUT_DEFAULT(...) \
    ,nullptr

#define _CONSTRUCT_CONFIG__INIT_MEMBERS(unused, data, idx, elem) \
    BOOST_PP_COMMA_IF(idx) \
        ::config_ctor::get_value<BOOST_PP_TUPLE_ELEM(0, elem)>( \
             BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(1, elem)) \
            ,cfg \
            BOOST_PP_IF( \
                 BOOST_PP_GREATER_EQUAL(BOOST_PP_TUPLE_SIZE(elem), 3) \
                ,_CONSTRUCT_CONFIG__INIT_MEMBERS_WITH_DEFAULT \
                ,_CONSTRUCT_CONFIG__INIT_MEMBERS_WITHOUT_DEFAULT \
            )(elem) \
        )

#define _CONSTRUCT_CONFIG__ENUM_MEMBERS(unused, data, idx, elem) \
    os << BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(1, elem)) "="; \
    ::config_ctor::print_value< \
        std::is_arithmetic<decltype(BOOST_PP_TUPLE_ELEM(1, elem))>::value \
    >::print(BOOST_PP_TUPLE_ELEM(1, elem), os); \
    os << std::endl;

/***************************************************************************/

#define _CONSTRUCT_CONFIG__ENUM_WRITE_MEMBERS(unused, data, idx, elem) \
    ptree.put(BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(1, elem)), cfg.BOOST_PP_TUPLE_ELEM(1, elem));

/***************************************************************************/

#define _CONSTRUCT_CONFIG__GENERATE_STRUCT(fmt, name, seq) \
    struct name { \
        BOOST_PP_SEQ_FOR_EACH_I( \
             _CONSTRUCT_CONFIG__GENERATE_MEMBERS \
            ,~ \
            ,seq \
        ) \
        \
        static name read(std::istream &is) { \
            boost::property_tree::ptree cfg; \
            boost::property_tree::read_##fmt(is, cfg); \
            \
            return { \
                BOOST_PP_SEQ_FOR_EACH_I( \
                     _CONSTRUCT_CONFIG__INIT_MEMBERS \
                    ,~ \
                    ,seq \
                ) \
            }; \
        } \
        static name read(const std::string &fname) { \
            boost::property_tree::ptree cfg; \
            boost::property_tree::read_##fmt(fname, cfg); \
            \
            return { \
                BOOST_PP_SEQ_FOR_EACH_I( \
                     _CONSTRUCT_CONFIG__INIT_MEMBERS \
                    ,~ \
                    ,seq \
                ) \
            }; \
        } \
        \
        static void write(const std::string &fname, const name &cfg) { \
            boost::property_tree::ptree ptree; \
            BOOST_PP_SEQ_FOR_EACH_I( \
                 _CONSTRUCT_CONFIG__ENUM_WRITE_MEMBERS \
                ,~ \
                ,seq \
            ) \
            boost::property_tree::write_##fmt(fname, ptree); \
        } \
        static void write(std::ostream &os, const name &cfg) { \
            boost::property_tree::ptree ptree; \
            BOOST_PP_SEQ_FOR_EACH_I( \
                 _CONSTRUCT_CONFIG__ENUM_WRITE_MEMBERS \
                ,~ \
                ,seq \
            ) \
            boost::property_tree::write_##fmt(os, ptree); \
        } \
        \
        std::ostream& dump(std::ostream &os) const { \
            BOOST_PP_SEQ_FOR_EACH_I( \
                 _CONSTRUCT_CONFIG__ENUM_MEMBERS \
                ,~ \
                ,seq \
            ) \
            \
            return os; \
        } \
    };

/***************************************************************************/

#define CONSTRUCT_CONFIG(\
     fmt  /* config file format */ \
, name /* config struct name */ \
, seq  /* sequence of vars */ \
) \
    _CONSTRUCT_CONFIG__GENERATE_STRUCT( \
         fmt \
        ,name \
        ,BOOST_PP_CAT(_CONSTRUCT_CONFIG__WRAP_SEQUENCE_X seq, 0) \
    )

#define CONSTRUCT_INI_CONFIG(name, seq) \
    CONSTRUCT_CONFIG(ini, name, seq)

#define CONSTRUCT_JSON_CONFIG(name, seq) \
    CONSTRUCT_CONFIG(json, name, seq)

#define CONSTRUCT_XML_CONFIG(name, seq) \
    CONSTRUCT_CONFIG(xml, name, seq)

#define CONSTRUCT_INFO_CONFIG(name, seq) \
    CONSTRUCT_CONFIG(info, name, seq)

/***************************************************************************/

#ifdef CONFIG_CTOR_HEADER_ONLY
#   include <config-ctor/config-ctor.cpp>
#endif // 

#endif // __config_ctor__config_ctor_hpp
