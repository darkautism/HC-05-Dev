#  Custom firmware for HC-05/06 to connect DS4

Please see [arduino example](arduino_example) or [golang example](golang_example)

If you wanna find pre-compiled firmware, please see [release](https://github.com/darkautism/HC-05-Dev/releases)

## USB Mode guide

This custom firmware will allow you to use HC-05/HC06 as a Dualshock 4 dongle.

![pinout](images/pinout.png)

First, you should weld D+ and D- pin. And buy a USB type A connector.

![weld](images/weld.jpg)

Burn the custom firmware into your HC-06/HC-05. Plugin this usb into windows.

![devicemapper](images/devicemanager.png)

Game controller can recognize our device now!

![ds4windows](images/ds4windows.png)

## Know issue

- Input delay too high (more than 100ms)