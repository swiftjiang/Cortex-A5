SLABHIDtoUART Library for CP2110/CP2114
---------------------------------------

Release packages contain binaries built using Ubuntu 11.10 Desktop 32-bit and
Ubuntu 12.04.1 Desktop 64-bit. These binaries may not be compatible with your
system, in which case it may be necessary to build the SLABHIDtoUART and
HidUartExample binaries using make.

SLABHIDtoUART linux library release contents
--------------------------------------------

- /Common/        - Shared/common source
- /HIDAPI/        - HIDAPI source (HID library that uses libusb-1.0)
- /nbproject/     - Netbeans project that uses the library and test main app
                    makefiles
- /Release/       - SLABHIDtoUART library header files
- /Release/Linux/ - Output directory for main and libslabhidtouart.so.1.0
                    (released with prebuilt 32-bit Ubuntu 11.10 binaries)
- /Release/Linux/x86_64 - Contains 64-bit binaries (released with prebuilt
                          64-bit Ubuntu 12.04.1 binaries)
- /SLABHIDDevice/ - Silicon Labs HID API source
- /SLABHIDtoUART/ - Silicon Labs HIDtoUART API source
- /main.cpp       - Basic test app that compiles HIDtoUART source w/o using
                    libslabhidtouart.so
- /Makefile-lib-linux   - libslabhidtouart.so.1.0 makefile
- /Makefile-main-linux  - main makefile
- /FAQ.txt              - Frequently asked questions regarding building and using
                          the SLABHIDtoUART library and HidUartExample.
- /ReadMe.txt           - Explains how to build and use the SLABHIDtoUART library
- /SharedObjectLibraryUsage.txt - Explains how to use libslabhidtouart.so
- /SiliconLabs.rules    - udev rules file for Silicon Labs USB devices
                          (VID = 0x10c4)

HidUartExample release contents
-------------------------------

- /nbproject/     - Netbeans project that uses the makefile
- /Release/Linux/ - Output directory for hidUartExample
                    (released with prebuilt 32-bit Ubuntu 11.10 hidUartExample)
- /main.cpp       - The main source file for hidUartExample
- /Makefile       - The makefile for hidUartExample
- /ReadMe.txt     - Explains how to build and run hidUartExample
- /SharedObjectLibraryUsage.txt - Explains how to use libslabhidtouart.so
- /SiliconLabs.rules    - udev rules file for Silicon Labs USB devices
                          (VID = 0x10c4)
- /SLABHIDtoUart.h - libslabhidtouart library include file
- /Types.h         - shared types header file
- /Terminal.cpp    - Linux terminal helper functions
- /Terminal.h      - Linux terminal helper function prototypes

Dependencies
------------

1. libusb-1.0

On Ubuntu:

  $ sudo apt-get install libusb-1.0-0-dev

2. g++

On Ubuntu:

  $ sudo apt-get install g++

Build SLABHIDtoUART
-------------------

Build an example console app, main, that uses the SLABHIDtoUART source files to
communicate with a CP2110/CP2114:

  $ make -f Makefile-main-linux

  Builds /Release/Linux/main

Build libslabhidtouart.so.1.0, a shared object library that exports the
SLABHIDtoUART library API

  $ make -f Makefile-lib-linux

  Builds /Release/Linux/libslabhidtouart.so.1.0

  See SharedObjectLibraryUsage.txt for more information on how to use a shared
  object library.

Configure Your System to Allow libusb to Access Your Device
-----------------------------------------------------------

SLABHIDtoUART uses HIDAPI to communicate with the CP2110/CP2114 over HID USB.
HIDAPI requires libusb-1.0 in order to communicate with USB devices. In order for
SLABHIDtoUART to recognize your device, you must first copy a udev file to
/etc/udev/rules.d/. This udev file will automatically enable read and write
permissions for a device with the specified USB vendor ID. Unplug and replug your
device after copying a udev file for the permissions to take effect. On some
systems, it may be necessary to reboot the machine before the new rules are
applied.

CP2110 (VID: 0x10c4 PID: 0xea80):
CP2114 (VID: 0x10c4 PID: 0xeab0):
- Copy SiliconLabs.rules to /etc/udev/rules.d/

Other:
- Modify an existing udev rules file for the required vendor ID

  SUBSYSTEM=="usb", ATTRS{idVendor}=="10c4", MODE="0666"
  SUBSYSTEM=="usb_device", ATTRS{idVendor}=="10c4", MODE="0666"
  
  - or - (optionally add a required product ID)

  SUBSYSTEM=="usb", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea80", MODE="0666"
  SUBSYSTEM=="usb_device", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea80", MODE="0666"
  
- SiliconLabs.rules does not specify the product ID and will therefore allow
  read/write access to all Silicon Labs USB devices with the Silicon Labs VID (0x10c4).
  The product ID is optional and will further restrict which devices the rules file
  will affect.
