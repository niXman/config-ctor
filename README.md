[![Build Status](https://travis-ci.org/niXman/config-ctor.svg?branch=master)](https://travis-ci.org/niXman/config-ctor)

config-ctor
===========

Configuration reader/writer constructor.

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
