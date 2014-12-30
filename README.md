libcanusb
=========
Library for communicating with CANUSB adapters http://www.can232.com/canusb/

Currently only supports VCP (virtual serial port) mode, but D2XX support is planned.

An example program is provided which opens the CAN channel and dumps all data according to the CANUSB protocol to stdout and log.txt.

Dependencies
------------
Raspberry Pi running Arch:
> pacman -S gcc make

Compiling
------------
> make

Running
------------
Example:
> ./bin/canusb /dev/ttyUSB0
