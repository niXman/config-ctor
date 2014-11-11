
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
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/enum.hpp>
#include <boost/preprocessor/tuple/pop_front.hpp>

/***************************************************************************/

namespace construct_config {

static std::size_t format_size(std::string str) {
	const char s = str.back();

	std::size_t mult = 1;
	switch ( s ) {
		case 'g': mult *= 1024u;
		case 'm': mult *= 1024u;
		case 'k': mult *= 1024u;

		str.pop_back();
	}

	return std::stoul(str)*mult;
}

template<bool ok>
struct get_concrete_value {
	template<typename T>
	static T get(const char *key, const boost::property_tree::ptree &ini) {
		return ini.get<T>(key);
	}
};

template<>
struct get_concrete_value<true> {
	template<typename T>
	static T get(const char *key, const boost::property_tree::ptree &ini) {
		const std::string val = ini.get<std::string>(key);
		return format_size(val);
	}
};

template<typename T>
static T get_value(const char *key, const boost::property_tree::ptree &cfg) {
	using TT = typename std::remove_const<T>::type;
	enum { is_numeric = std::is_integral<TT>::value && !std::is_same<TT, bool>::value };
	return get_concrete_value<is_numeric>::template get<TT>(key, cfg);
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
		if ( std::is_same<T, bool>::value ) {
			os << (v ? "true" : "false");
		} else {
			os << v;
		}
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
	BOOST_PP_TUPLE_ENUM(BOOST_PP_TUPLE_POP_FRONT(elem)) /* type */ \
		BOOST_PP_TUPLE_ELEM(0, elem); /* var name */

#define _CONSTRUCT_CONFIG__GENERATE_INITIALIZER_LIST(unused, data, idx, elem) \
	BOOST_PP_COMMA_IF(idx) BOOST_PP_TUPLE_ELEM(0, elem) ()

#define _CONSTRUCT_CONFIG__INIT_MEMBERS(unused, data, idx, elem) \
	BOOST_PP_COMMA_IF(idx) \
		::construct_config::get_value<BOOST_PP_TUPLE_ENUM(BOOST_PP_TUPLE_POP_FRONT(elem))>( \
			BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(0, elem)) \
			,cfg \
		)

#define _CONSTRUCT_CONFIG__ENUM_MEMBERS(unused, data, idx, elem) \
	os << BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(0, elem)) "="; \
	::construct_config::print_value< \
		std::is_integral<decltype(BOOST_PP_TUPLE_ELEM(0, elem))>::value \
	>::print(BOOST_PP_TUPLE_ELEM(0, elem), os); \
	os << std::endl;

/***************************************************************************/

#define _CONSTRUCT_CONFIG__ENUM_WRITE_MEMBERS(unused, data, idx, elem) \
	ptree.put(BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(0, elem)), cfg.BOOST_PP_TUPLE_ELEM(0, elem));

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
		}\
		static void write(std::ostream &os, const name &cfg) { \
			boost::property_tree::ptree ptree; \
			BOOST_PP_SEQ_FOR_EACH_I( \
				 _CONSTRUCT_CONFIG__ENUM_WRITE_MEMBERS \
				,~ \
				,seq \
			) \
			boost::property_tree::write_##fmt(os, ptree); \
		}\
		\
		void dump(std::ostream &os) const { \
			BOOST_PP_SEQ_FOR_EACH_I( \
				 _CONSTRUCT_CONFIG__ENUM_MEMBERS \
				,~ \
				,seq \
			) \
		}\
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
