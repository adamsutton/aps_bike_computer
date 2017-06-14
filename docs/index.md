Introduction
============

This is my attempt to build a simple bike computer. My reasons for doing this
are really two fold:

1. It's a bit of fun. A chance to play with some electronics (not my strongest
   point) and to write some embedded code (my day job, but I love it).

2. Because my existing cycle computer (Bryton Rider 40) has some minor issues
   and this will allow me to overcome them without becoming a slave to the
   overly expensive Garmin products.

Plan
====

The original plan is just to get a very basic GPS logger to get 1Hz data points (one of my issues with the Bryton is it's 1/4Hz update). And log this to flash.

Beyond that and a battery to power it I really wasn't intending to do much more than that initially.

To that end I purchased an STM32F103 discovery board (as I've been using STM32's for quite some time and this one in particular of late) and a ublox GPS module.

However my brother persuaded me that I had to add ANT+ (and BLE if I could as his power meter is BLE) so I found a Nordic Semi chip that included what I needed and a handy SparkFun breakout board with it on.

However this is a fully blown SOC with the radio system being provided as a softcore to run on the Cortex-M4. So potentially that puts an end to the STM32.

And I've also now progressed to using microSD (via SPI) with FAT filesystem for the data storage. This means that initially I can just sync the data to my computer by removing the microSD and putting straight into my laptop. Although eventually it would be good to turn the device into a USB mass storage device.

So now the plan is a little more grandiose. However step 1 is still to get basic GPS logging to micro SD and no more than that.


Hardware
========

- STM32F103 (ST Nucleo board - still using this for now until nRF52 is working)
- Nordic Semi nRF 52832 (on SparkFun 13990)
- Ubloc NEO-6m
- MicroSD (on breakout board)
- Dallas 1-wire temperature sensor (DS18B20)

Software
========

Most of the application and glue code I'll write myself. However unlike with former work projects I've decided I'm not going to write all the bare metal drivers myself and will relay on any support layers available (like STM Standard Peripheral library, or equivalent).

And if there are good 3rd party libraries available I'll use those:

- STM32 std peripheral library
- PicoFAT (http://elm-chan.org/fsw/ff/00index_p.html)

And I'm using Eclipse for development using the various GNU ARM plugins to provide toolchain, debugger, etc...

