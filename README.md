# log4cxx : Log system like log4js for C++
---
Copyright &copy; 2016 SiLeader. All rights reserved.  
Licensed under [MIT License](https://opensource.org/licenses/MIT).  

## Dependent Libraries
C++ Standard Library(C++11 or more version)  
[PicoJSON(picojson.h)](https://github.com/kazuho/picojson)

## Version
1.0.0-release  

## Introduction
log4cxx is log system like log4js for C++  
feature  
* only one header file.
* preference can use log4js's preference. (log4cxx has extension preference)  

## Sample Code
`"category":"log"`  
Use default preference name(log_config.json)
```
log4cxx::log4cxx l4c();
log4cxx::logger log=l4c.get_logger("log");
log.info("Log");
```
log file :
```
[2016:01:01 00:00:00.000000000] [info] log - Log
```
