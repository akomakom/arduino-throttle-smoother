Arduino Throttle Correction for 3-wire thottles
----

### Purpose

This project is suitable for electric vehicles with a non-programmable controller. 
The arduino is inserted between a 3-wire throttle and the controller.

* Provides smoothing for jittery throttle response by slowing down changes in output, slightly
* Reduces deadband by mapping input to output values as per configuration
* Optionally provides an adjustable speed limit

### Wiring and Implementation

![arduino-throttle-wiring.png](arduino-throttle-wiring.png)

Wiring is very simple.  Smoothing capacitor can be any value that eliminates
jumping of motor speed at steady throttle.  I believe that I used a 100uF for my setup.

![arduino-throttle.png](arduino-throttle.png)
Note that I added a power supply capacitor as well, but I don't think it was necessary.

### Tuning

Tune behaviors according to your throttle's actual values, which you can see if you watch serial output.

If you don't want speed limiting, comment out `#LIMIT_ENABLE`

**Note** that when plugged into USB, Arduino is running at 5V, but when powered by the motor controller,
it is most likely running on 4.0V-4.5V.  This changes the numbers and behavior a little when you unplug USB.