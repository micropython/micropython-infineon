# The XMC port

This is a MicroPython port that runs on the Infineon XMC4700 Relax Kit.

## Building

    $ make

## Starting J-Link GDB Server

For the On-Chip Debugger you will need to install the [J-Link Software and Documentation Pack](https://www.segger.com/downloads/jlink). Connect USB cable to the debugger connection X101.

To start:

    $ JLinkGDBServer -Device XMC4700-2048 -if SWD

## Running

Once the J-Link GDB Server runs, you can load the code on the board and start the GDB session using:

    $ make install
