[![Build Status](https://travis-ci.org/niXman/config-ctor.svg?branch=master)](https://travis-ci.org/niXman/config-ctor)

config-ctor
===========

Configuration reader/writer constructor based on [boost.property_tree](http://www.boost.org/doc/libs/1_59_0/libs/property_tree/index.html).

Currently the `ini`, `json`, `xml`, `info` formats are supported.


Tutorial
===========
Write the info about options you need:
```cpp
CONSTRUCT_INI_CONFIG(
	myconfig, // the name of the generated type(struct)
	(int, a) // the sequence of the variables
	(double, b)
)
```
Read the options from the `.ini` file:
```cpp
myconfig cfg = myconfig::read("myconfig.ini");
```
Write the options to the `.ini` file:
```cpp
myconfig cfg;
cfg.a = 33;
cfg.b = 44.55;
myconfig::write("myconfig.ini", cfg);
```

Default values
===========
You can specify the default values for options. To do this, specify the third parameter for this variable:
```cpp
CONSTRUCT_INI_CONFIG(
	myconfig, // the name of the generated type(struct)
	(int, a, 33) // the sequence of the variables
	(double, b, 44.55)
)
```

Placeholders
===========
Also you can use placeholders in your config files.
For example, this(`stat_log={HOME}/procs/{PID}/stat.log`) line in config file will be readed as `stat_log=/home/nixman/procs/18151/stat.log`

The following placeholders are supported:
 - `{USER}            // user name`
 - `{HOME}            // user home path`
 - `{CWD}             // current path`
 - `{TEMP}            // user temp path`
 - `{PID}             // process ID`
 - `{PATH}            // PATH env`
 - `{PROC}            // executable path`
 - `{GETENV(ENV_VAR)} // gets the system env var value. also you can use it with default value: {GETENV(ENV_VAR, defaul_value)}`
