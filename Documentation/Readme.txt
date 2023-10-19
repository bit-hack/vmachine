VMachine
========

Copyright (c) 2006, Paul Baker

http://www.paulsprojects.net

VMachine is a PC emulator, a recreation of a PC in software.
Running on your PC, it creates a completely separate virtual machine.

This allows you to run multiple operating systems concurrently on a single machine, giving you the ability to test software on multiple configurations, perform potentially risky operations in an isolated environment or simply indulge in some retro gaming.

Usage
-----

VMachine requires a memory size and a hard disk image to be specified on the command line. A floppy disk image may also be specified if required. These are set using the command line options:

-mem: Memory size
-fdimg: Floppy disk image
-hdimg: Hard disk image

An initial set of command line options is specified in VMachine.bat. To use VMachine with these options, simply double-click on the batch file.

VMachine accepts keyboard and mouse input, mapping the host's input devices to the virtual keyboard and mouse.

The virtual keyboard does not distinguish between the main arrow keys and those on the numeric keypad. Also, the state of NumLock on your host keyboard is not always correctly reflected in the virtual keyboard. This can lead to keypresses of the arrow keys being registered as numbers. If this occurs, toggle the "Num Lock" setting on your host keyboard. This will be reflected on the virtual keyboard, and the arrow keys will then work as expected.

To use the mouse, simply click on the VMachine window. The mouse will then be "captured", and will control the guest mouse cursor (if any), rather than the host cursor. To release the mouse, press Ctrl and Alt together.

VDisk
-----

The VDisk tool allows you to create new floppy disk and hard disk images. Simply specify the type of image you wish to create and a filename. A hard disk image also requires a size to be specified, between 20 and 500 MB. All floppy disk images used by VMachine are of size 1.44MB.

FreeDOS sample
--------------

VMachine comes with a small sample hard disk image containing a basic installation of FreeDos (http://www.freedos.org).

Further Information
-------------------

A list of the software VMachine has been tested with can be found in "Compatibility.txt".
For performance data, see "Performance.txt".
For detailed information about the emulation methods used in VMachine, see "Technical.txt".

VMachine is open source software, see "License.txt" for details.
