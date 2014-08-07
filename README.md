The TUSB3410 USB stack was loosely based on Micah Dowty's polling based
USB driver, but was heavily modifed by me using TI's app notes and
source code examples. The resulting code is a mix of interrupt code and
polling. 

This is a clone of Altera USB Blaster based on the work of Kolja Waschk,
ixo.de. The code is ported to TI TUSB3410 USB to Serial converter chip
which is found in TI's MSP430 programmers and some very old cell phone
USB Serial interface dongles. 

TUSB3410 has a very slow microcontroller core. There are much better
chips out there. This is meant to serve as a demo of the USB stack and
to show that the chip can works as a USB JTAG device for OpenOCD and USB
Blaster clone in less than 3kB firmware.

TUSB3410 has onchip SRAM for firmware and you'll need to use my command
line util: load3410.exe to download its firmware.  Use libusb to make up a
.inf file/driver for it to stop window popup asking for drivers.

TI's apps loader util can also be used as an alternative.

License: Individual source files without copyright notices are written
by me and are distributed under GPL ver 2 or newer.

TUSB source code is compiled with SDCC 3.3.  I used Programmer's Notepad
as my IDE.  Win32 Util can be compileed with Pelles C for windows.

TUSB Blaster schematic/layout are Eagle CAD 6.5.0 files.

