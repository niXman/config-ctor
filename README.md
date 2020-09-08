[![Build Status](https://travis-ci.org/niXman/config-ctor.svg?branch=master)](https://travis-ci.org/niXman/config-ctor)

config-ctor
===========

C++ preprocessor type constuctor for processing structured (JSON) configuration files.

Tutorial
===========
Describe the info about options you need:
```cpp
CONSTRUCT_CONFIG(
    myconfig, // the name of the generated type(struct)
    (int, a) // the sequence of the variables
    (double, b)
)
```
For read the options from file:
```cpp
myconfig cfg = myconfig::read("myconfig.cfg");
```
For write the options to file:
```cpp
myconfig cfg;
cfg.a = 33;
cfg.b = 44.55;
myconfig::write("myconfig.cfg", cfg);
```

Nested config-ctor types
===========
As members now you can use nested config-ctor types:
```cpp
CONSTRUCT_CONFIG(
    option,
    (int, i)
    (std::string, s)
)
CONSTRUCT_CONFIG(
    myconfig, // the name of the generated type(struct)
    (int, a) // the sequence of the variables
    (double, b)
    (option, o) // <<<<<<<<<<<<<<<<<<<<<<
)
```

Default values
===========
You can specify the default values for options. To do this, specify the third parameter for that variable:
```cpp
CONSTRUCT_CONFIG(
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
