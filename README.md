# TDOS - A tapedeck OS

## ABOUT

This is a collection of libraries to use an old tapedeck as control
device.

It's pronounced `T.D. O.S.` or `Tapedeck OS` (but not `T. DOS`).

It is not a real OS. Sorry.

## WHY

Old tapedecks are great looking devices from a time where it was
possible to get electronics documentation and schematics for it and
the electronics is accessible to soldering and replacement, all
enabling you to get it repaired. Separate from copyright, that's
nearly what "open hardware" tries to achieve again nowadays.

The visuals and haptics of those devices are so gorgeous that it would
be a pity to abandon them just because you don't listen to your old
tapes anymore.

## How it works

The tapedeck's electronics are hijacked by soldering wires from all
major buttons and potentiometers, yet preserving its functionality.

Those wires are connected to an Arduino and or a Raspberry Pi,
depending on the task. At least analog wires are read or set by an
arduino, and maybe everything is done on the arduino except when
running out of memory or the C-like programming gets too silly or I'm
unable to get it abstract enough to be reused with other tapedeck
controls, or all of it. It all depends on my ability to learn
programming in reduced environments after decades of memory
decadence. In case not everything works on the arduino we only provide
a communication channel to the arduino so only the actual signals are
processed there and all logic is done outside.

The electronics of the tapedeck are decoupled from the arduino with
optocouplers. One exception might be when controlling the peak
programme meter, but that's a not yet solved problem anyway.

# Further reading

## Technics RS-B49R (1985)

This is the initial tapedeck I used for this project.

- https://www.cassettedeck.org/technics/rs-b49r
- https://www.hifiengine.com/manual_library/technics/rs-b49r.shtml

## Resources

### Electronics in general

- https://en.wikipedia.org/wiki/Peak_programme_meter

### Arduino

- https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
- https://www.mikrocontroller.net/articles/Verwendung_von_Interrupts_mit_Arduino
- file:///usr/share/doc/arduino-core/reference/index.html
- http://www.robert-fromm.info/?post=elec_spi_mcp3008
- https://www.teachmemicro.com/wp-content/uploads/2019/06/Arduino-Nano-pinout-1.jpg


### Raspberry Pi

# Random notes

## Connecting to serial console of Arduino

### Cmdline but fragile

```
cu --parity=none --nostop -s 9600 -l /dev/ttyUSB0         # or /dev/ttyUSB1 etc.
```

### From arduino sketch IDE

- `arduino &`
- Tools -> Serial Monitor (Ctrl+Shift+M)
