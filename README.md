libcanusb
=========
Library for communicating with CANUSB adapters http://www.can232.com/canusb/

Currently only supports VCP (virtual serial port) mode, but D2XX support is planned.

An example application can be found at https://github.com/Chetic/btmis

Dependencies
------------
- http://www.ftdichip.com/Drivers/D2XX.htm
Raspberry Pi running Arch:
> pacman -S gcc make

Compiling
------------
> make

The static library is placed in lib/libcanusb.a
