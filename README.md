# NiroClock
Amazing clock that looks like dumb terminal, but actually very smart.
[![View PCB on KiCanvas](https://hack.club/pcb-badge)](https://kicanvas.org/?repo=https%3A%2F%2Fgithub.com%2FNurazkhan%2FNiroClock%2Ftree%2Fmain%2Fpcb)
![NiroClock](<assets/3denclosure.png>)

I made this clock with the help of hackClub. You can find useful references/examples in [BlareV2]([url](https://stardance.hackclub.com/missions/blare)) section.

## Features
* 76x284px for different modes of time display.
* 5 Cherry MX style Keys for best experience.
* 3D printed perfect case that looks like old computer giving +++Aura
* .ino firmware containing 5 pages and each page with button control.
* the noisy buzzer that won't let you sleep after alarm.

## CAD Model
CAD consists of a total of 4 parts: Main bottom, main cover, back plate, front plate.
The case and the cover for it fit together with 4*M3 screws and heatset inserts.
The back plate is also attached using 2*M3 and well as front plate.
The case has space for keys and display, so it fits perfectly.
![NiroPad](<assets/3dback.png>)
Made in OnShape. by Nurazkhan (me).

## PCB
![NiroPad](<assets/Scheme.webp>)
Schematic was made in KiCad.
![NiroPad](<assets/routepcb.png>)
PCB routing was also made in KiCad Software. Footprints for keys were imported from recommended resources; you can also find [it]([url](https://github.com/sszczep/kicad-libraries)).

## Firmware Overview
The NiroPad Works on c++ used in arduino IDE
Main functions are:
* 5 keys for clock control.
* buzzer to wake you up.
* 5 different pages qith unique control.
* change display mode from HH:MM:SS or HH:MM.
* Start Secondmeter


## BOM
* Cherry MX style switches	5
* Seeed XIAO ESP32C3	1
* Blank DSA caps	5
* 2.25in TFT Screen	1
* Custom PCB	1
* 3.3V Piezo Buzzer	
* 3d printed enclosure	4
* 2.54mm 8 Pin Male Header	1
* M3x16mm 	4
* M3x8mm 	4
* M3x5x4 Heatset Inserts	8
* 20cm Female-Female Jumper Wires	8
 

The process of designing and creation you can see in the ![website](https://stardance.hackclub.com/projects/63343)

Thank you!
#hackClub

