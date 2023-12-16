## Minimal Weather Station



Minimal weather station based around hobbiest-grade sensors and cadalogger mini logging 
platform.

Rain gauge, wind vane and anemometer:
https://thepihut.com/products/wind-and-rain-sensors-for-weather-station-wind-vane-anemometer-rain-gauge

Temperature and humidity:
https://thepihut.com/products/sht31-weather-proof-temperature-humidity-sensor

cadalogger:
https://github.com/mbmorrissey/cadalogger

This combination requires only trivial further components (resistors and capacitors, 
battery holder, termimal
blocks) to generate a multi-parameter weather station for <£150.



### to-do

- check need to disable interrupts in set-up
- make power profile for current set-up
- record V drop in hardware debounce circuits
- get photo of deployment



### Alternate power supply possibilities

Current version is meant to run off of 4.5 V of standard batteries.  This is to support a 
SH student project
at a field site that currently does not have continuous local meterological data.  Power 
profile remains to be
done, but MBM suspects that continuous operation using, for e.g., 
https://www.tindie.com/products/jaspersikken/solar-harvesting-into-lithium-ion-capacitor/
with as little ad 0.2W panel could be a future addition.

### Thoughts on extensions for commercial and open-source alternate parts

This project could ultimately form the basis of a minimal design for general use 
low-investment environmental
monitoring applications.  Student will ground-truth against nearest met site for purpose 
of SH project. MBM
and RN and/or MJJDS to check anemometer pulses -> wind speed conversion (internet not 
consistent on info for this)
against co-deployment with sonic anemometer upon retrieval of one of the latter from 
current St Kilda deployment.
This rain gauge is known to underestimate in heavy rain or high wind, but is still likely 
to generate very 
meaningful relative data (e.g., periods with vs without rain).



Possible future work could give options for swapping sensors for various upgrades, all 
with full open-source
documentation/library/functions, and ground-truthing data, for field station/site-specific 
needs and budgets.
Options could include upgraded commercially available options or fully open-source, 
options.

https://www.printables.com/model/130513-rain-gauge
https://www.thingiverse.com/thing:3546977



### Crude hardware debounce seems to work well as part of low-power design

Design for anemometer and rain gauge uses reed switches with hardware de-bounce (100k 
pullup with 0.1 uF to 
ground).  This works well for very low-power operation, and MCU seems to detect each pulse 
despite the falling
edge being a bit slow (MBM to double check on scope and record result).
