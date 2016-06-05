What Am I
=================
I am a C/C++ API or library to write automation applications for the Raspberry Pi. I
ease the programming of applications by providing an abstraction layer with the most
common ready-to-use functions used in automation applications.

My main goal is to allow non C/C++ and linux experts to write reliable applications
to automate stuff. Examples could be home automation, automated gardens, hobbyists'
experimets, etc.

With me you can access GPIOS (inputs and outputs) and communication buses. You can
create high priority automation deamons and threads with defined cycle times. On top
of that, I provide logging functions to track errors, messages with state to avoid
spamming your log and much more.

I am free like free beer under the LGPL and aim to promote the idea of  _give something 
back to the world_.

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

## Functionality Goals
1. Core functions (sleeps, threads, soft real-time, timers, clocks)
  1. Configuration file parser
2. GPIOs
3. Communication (One Wire Bus, Serial, SPI, I2C)
4. Control structures (PID, PT1, debouncer, Schmitt trigger)
5. Error handling, logs, status reports
6. Control tasks with cycle time and priority

## Developers Documentation
[Read more](https://pettitpeon.github.io/rpiapi/Doc/html/index.html)
