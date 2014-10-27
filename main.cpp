
#include <config-ctor/config-ctor.hpp>

#include <iostream>
#include <cassert>

/***************************************************************************/

int main() {
	//////////////////////////////////////////////// ini
	{
		CONSTRUCT_INI_CONFIG(
			config,
			(a, int)
			(b, float)
			(c, std::string)
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

		const config rcfg = config::read(ss);
		assert(rcfg.a == a);
		assert(rcfg.b == b);
		assert(rcfg.c == c);
	}
	//////////////////////////////////////////////// json
	{
		CONSTRUCT_JSON_CONFIG(
			config,
			(a, double)
			(b, long)
			(c, std::uint16_t)
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

		const config rcfg = config::read(ss);
		assert(rcfg.a == a);
		assert(rcfg.b == b);
		assert(rcfg.c == c);
	}
	//////////////////////////////////////////////// xml
	{
		CONSTRUCT_XML_CONFIG(
			config,
			(a, double)
			(b, long)
			(c, std::uint16_t)
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

		const config rcfg = config::read(ss);
		assert(rcfg.a == a);
		assert(rcfg.b == b);
		assert(rcfg.c == c);
	}
	//////////////////////////////////////////////// info
	{
		CONSTRUCT_INFO_CONFIG(
			config,
			(a, double)
			(b, long)
			(c, std::uint16_t)
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

		const config rcfg = config::read(ss);
		assert(rcfg.a == a);
		assert(rcfg.b == b);
		assert(rcfg.c == c);
	}
}

/***************************************************************************/
