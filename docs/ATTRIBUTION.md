# Attribution and dependencies

I built this vehicle for ECE 306. Jim Carlson designed the schematics and layouts for the boards added to the pre-populated TI controller board. I populated the components and developed the vehicle firmware using course support material and TI components. Original component notices are retained.

| Component | Origin and role | Distribution |
| --- | --- | --- |
| Application firmware | ECE 306 project sources covering sensing, navigation, communication, timing and user interface | Included in `src/` and `include/`; existing author comments retained |
| Clock initialization | `src/clocks.c` contains a TI copyright notice in `Software_Trim()` | Included unchanged with the original notice |
| LCD driver | Course-provided `Compiled/LCD.obj` used by the original firmware | External dependency; binary not bundled |
| Device linker script | Texas Instruments, copyright 2012–2021 | Included unchanged under its original redistribution terms |
| Device headers and runtime | TI MSP430 toolchain, including `msp430.h`, `frwp_init.a`, and `libc.a` | Supplied by the toolchain installation |
