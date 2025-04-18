# iidx-twinkIO
Arduino implementation of Twinkle IO interface.

## What is this?
This project allows your arduino to control Sub-IO, or effector panel (16-segment displays) and top lights (spotlights, neons) on your Beatmania IIDX Deluxe cabinet, independently from an official IO, without rewiring anything.

Additionally, it also allows you to read slider values, and if your cab is from Twinkle era (grey door), front plate button inputs and turntable position as well (only if it doesn't have a bypass cable installed, which it probably does).

This project is helpful if:

* You are using a third-party or modified IO board that doesn't have the ability to control Sub-IO
* You have just the effector plate lying around and want to display something on the ticker

## How do I use it?
### Hardware
You will need:
* **Arduino** (or any compatible board)
* **RJ-45 breakout board** (or DIN8 if your cab uses that)
* **3 RS-422 modules** (you need 3 since the IO uses 3 TX line and 1 RX line)

***What if I don't have / want to buy 3 RS-422 modules?***

For TX line, you technically can get away with wiring the 5v digital output pin to A pin and 3.3v to B pin. **Do this at your own risk!**

For RX line, you would most likely want to use RS-422 module.

---

Wirings for the RJ-45 port is as follows:

| RJ-45 Pin | Description | Direction |
| :---: | :--- | :---: |
| 1 | Data Input Z | RX |
| 2 | Data Input Y | RX |
| 3 | Enable Output B | TX |
| 6 | Enable Output A | TX |
| 4 | Data Output B | TX |
| 5 | Data Output A | TX |
| 7 | Clock Output B | TX |
| 8 | Clock Output A | TX |

Wire each A-B / Y-Z pair to your RS-422 module of choice. A is wired to Y, and B to Z.

If you have a newer cab that uses DIN8 instead, you may try the [conversion pinout written in arcade-docs](https://github.com/shizmob/arcade-docs/blob/main/konami/io/GEC02/pwb-aa.md) and adapt it to match the pinout. I haven't tested it myself, but if anyone ends up getting this to work, please report back!

Now, wire TX/RX lines from RS-422 modules to arduino. The default pinout is as follows:
| Arduino Pin | Description
| :---: | :--- |
| 2 | Data Input |
| 3 | Enable Output |
| 4 | Data Output |
| 5 | Clock Output |

Your new IO board should be ready.

### Software
**Using Arduino IDE**

Save [`main.cpp`](./src/main.cpp) as `.ino` file, and upload it like usual. No external libraries are needed.

**Using PlatformIO**

The project is set to build for Arduino Pro Micro 5v, but it should work on other arduino-compatible board without any modification. Build and upload like you normally would (e.g. `pio run -t upload`).

## I wanna use slider inputs / control lights in my program!
While the code doesn't really implement communication with PC to send/receive data, serial works just fine and it still periodically reads all outputs from Sub-IO. You can probably write some code that will send these data to PC via serial.

## Resources
* [75ALS1178 datasheet](https://www.ti.com/lit/ds/symlink/sn75als1178.pdf)
  * 75ALS1178 is the RS-422 chipset used in Sub-IO.

* [M66009FP datasheet](https://www.alldatasheet.com/datasheet-pdf/pdf/1430/MITSUBISHI/M66009FP.html)
  * I/O expander used in Sub-IO. This chip is responsible for actually exchanging data, and 4 I/O pins on arduino directly drive this chip.

* [MAME implementation of TWINKLE PCB](https://github.com/987123879113/mame/blob/bemani/src/mame/konami/twinkle.cpp)
  * Has Sub-IO addresses and bit layout for M66009FP. Thanks  for the fantastic work!

* [Arcade Docs GEC02 documentation](https://github.com/shizmob/arcade-docs/blob/main/konami/io/GEC02/pwb-aa.md)
  

## Thanks to
* [@smf-](https://github.com/smf-) for MAME Twinkle PCB implementation, this was the most crucial resource!
* [@shizmob](https://github.com/shizmob) for maintaining an amazing community resource
* [Zenith Arcade](https://zenitarcade.com) for the resource and access to the cab
