
#ifndef _config_ctor_hpp
#define _config_ctor_hpp

#include <cstdint>
#include <string>
#include <iosfwd>

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

/***************************************************************************/

namespace construct_config {
namespace {

// for other types (strings)
template<bool ok>
struct get_concrete_value {
	template<typename T>
	static T get(const char *key, const boost::property_tree::ptree &ini, const char *default_value) {
		return default_value != nullptr
			? ini.get<T>(key, default_value)
			: ini.get<T>(key)
		;
	}
};

// specialization for arithmetic types
template<>
struct get_concrete_value<true> {
	template<typename T>
	static T get(const char *key, const boost::property_tree::ptree &ini, const char *default_value) {
		// handle bool types
		if ( std::is_same<T, bool>::value ) {
			return default_value != nullptr
				? ini.get<bool>(key, (std::strcmp(default_value, "true") == 0))
				: ini.get<bool>(key)
			;
		}

		// handle other arithmetic types
		std::string val = default_value != nullptr
			? ini.get<std::string>(key, default_value)
			: ini.get<std::string>(key)
		;
		if ( val.empty() ) return T{};

		if ( (std::is_signed<T>::value || std::is_unsigned<T>::value) && !std::is_floating_point<T>::value ) {
			std::size_t mult = 1u;
			switch ( val.back() ) {
				case 'G': mult *= 1024u;
				case 'M': mult *= 1024u;
				case 'K': mult *= 1024u;

				val.pop_back();
			}

			return (std::is_signed<T>::value ? std::stol(val) : std::stoul(val)) * mult;
		}

		return std::stod(val);
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

} // ns construct_config

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
		::construct_config::get_value<BOOST_PP_TUPLE_ELEM(0, elem)>( \
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
	::construct_config::print_value< \
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
		void dump(std::ostream &os) const { \
			BOOST_PP_SEQ_FOR_EACH_I( \
				 _CONSTRUCT_CONFIG__ENUM_MEMBERS \
				,~ \
				,seq \
			) \
		} \
	};

/***************************************************************************/

#define CONSTRUCT_CONFIG( \
	 fmt  /* config file format */ \
	,name /* config struct name */ \
	,seq  /* sequence of vars */ \
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

#endif // _config_ctor_hpp
