# README:
## General
1. This is a C/C++ API for the Raspberry Pi.
2. It is being developed with the Raspberry Pi 2 Model B, Raspbian Jessie
  1. Compiler: SysGcc cross-compiler arm-linux-gnueabihf-4.9.2
  2. You can get it here: http://gnutoolchains.com/raspberry/
3. The project file included is from eclipse CDT LUNA
 
## Architecture
- This API is based on small libraries with a couple of headers each.
- The external headers of each library contain only pure C functions and or a C++ pure virtual classes with its respective pure C factories
- Each library is in its own folder and has its respective unit test application
- The CppUnit test framework is included in this project and is always statically linked to the test applications

## Values
1. User friendly API
2. Mantainability
3. Extensibility
4. Backwards compatibility

## Goals
1. Core functions (sleeps, threads, soft real-time, timers, clocks)
  1. Configuration file parser
2. GPIOs
3. Communication (Serial, SPI, I2C)
4. Control structures (PID, PT1, debouncer, Schmitt trigger)
5. Error handling, logs, status reports
 
## Depelopers Documentation
[Read more](https://pettitpeon.github.io/rpiapi/Doc/html/index.html)