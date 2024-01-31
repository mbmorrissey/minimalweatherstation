![](https://github.com/mbmorrissey/minimalweatherstation/blob/main/images/scene_minimal.jpg)

## Minimal Weather Station


Minimal weather station based around hobby-grade sensors and the cadalogger data logging 
platform.

Rain gauge, wind vane and anemometer:

https://thepihut.com/products/wind-and-rain-sensors-for-weather-station-wind-vane-anemometer-rain-gauge
https://learn.sparkfun.com/tutorials/weather-meter-hookup-guide

Temperature and humidity:

https://thepihut.com/products/sht31-weather-proof-temperature-humidity-sensor

cadalogger:

https://github.com/mbmorrissey/cadalogger

This combination requires only trivial further components (resistors and capacitors, 
battery holder, termimal blocks) to generate a multi-parameter weather station for <£200 
(including cost of 
structural materials for deployment).



### to-do

- check need to disable interrupts in set-up
- record V drop in hardware debounce circuits
- photos of deployment


### Alternate power supply possibilities

Current version is meant to run off of 4.5 V of standard batteries.  This is to support a 
SH student project at a field site that currently does not have continuous local 
meterological data.  Power profile remains to be done, but MBM suspects that continuous 
operation using, for e.g., 

https://www.tindie.com/products/jaspersikken/solar-harvesting-into-lithium-ion-capacitor/

with as little as a 0.4W panel could be a future addition.

### Power consumption

Most power consumption is dominated by the write to the uSD card every minute, with a lesser contribution
from blinking the LED every 5s:

![](https://github.com/mbmorrissey/minimalweatherstation/blob/main/testing_data/minimalweatherpower1.png)

Zooming in on a 5s interval, we can see the alternation between power total power consumtion of about 30 uA
and 60 uA as the anemometer's reed switch opens and closes.  Of the two larger peaks, the first is the 
flash of the LED, and the second is the brief calculations done by the logger every 5s:

![](https://github.com/mbmorrissey/minimalweatherstation/blob/main/testing_data/minimalweatherpower2.png)

Average power consumption over the 1 minute cycle is about 290 uA.  On paper, an approx 2000 mAh AA battery
pack should then last 2000 mAh / 0.29 mA = approx 6890 h = approx 9 months.  We are currently testing what
fraction of this is realised under real-world conditions.



### Thoughts on extensions for commercial and open-source alternate parts

This project could ultimately form the basis of a minimal design for general use 
low-investment environmental monitoring applications.  Student will ground-truth against 
nearest met site for purpose of SH project. MBM and RN and/or MJJDS to check anemometer 
pulses -> wind speed conversion (internet not consistent on info for this) against 
co-deployment with sonic anemometer upon retrieval of 
one of the latter from current St Kilda deployment.  This rain gauge is known to 
underestimate in heavy rain or high wind, but is still likely 
to generate very meaningful relative data (e.g., periods with vs without rain).



Possible future work could give options for swapping sensors for various upgrades, all 
with full open-source documentation/library/functions, and ground-truthing data, for field 
station/site-specific needs and budgets.

Options could include upgraded commercially available options or fully open-source, 
options, e.g., 3D printable items such as:

https://www.printables.com/model/130513-rain-gauge

https://www.thingiverse.com/thing:3546977



### Crude hardware debounce seems to work well as part of low-power design

Design for anemometer and rain gauge uses reed switches with hardware de-bounce (100k 
pullup with 0.1 uF to ground).  This works well for very low-power operation, and MCU seems to detect each pulse 
despite the falling edge being a bit slow (MBM to double check on scope and record result).  Testing has not 
suggested any missed pulses from slow fall time, but improved hardware debounce (e.g., add Schmitt trigger) to 
be kept in mind.  Current thinking is that software debounce would lead to more MCU awake time, but detailed 
study of trade-offs between different debounce strategies has not been conducted.


### Example results

![](https://github.com/mbmorrissey/minimalweatherstation/blob/main/images/prelim_weather.jpeg)

Example data from a deployment in SW Scotland during the winter of 2023/4.
