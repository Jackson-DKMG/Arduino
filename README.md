# Arduino
Couple arduino scripts that are worth saving.

<b>Lightning</b> = simulate a thunderstorm with 5 leds. Requires building a "cloud" with a wire frame covered in translucent paper, then glue some cotton wool over it. 
It make a surprisingly credible lightning storm actually.

Demo here : https://drive.google.com/file/d/1y8FLZ6Q0NNzXq3QAmT5BpMyESNXeApRW/view?usp=sharing

<b>I2C clock</b> = So. It is a clock with an arduino nano or mini (don't care, the only significant difference is that one has an USB port).<br>
Should be the 16Mhz model though, as the 8 Mhz version does not have enough memory to store the program.<br>

The assembly is mounted on a leather wristband (extremely uncomfortable to wear indeed), and another version was on a motorcycle jacket.

I had a circuit diagram somewhere but anyway. A DS3231 RTC module (the actual clock) is wired to the I2C pins on the arduino, as well as an OLED screen (0.96", 128x64 I think).<br>
There is a led that flashed upon an alarm or when the battery is low, as well as a vibrator for the alarm and haptic feedback.<br>
4 buttons allow to set the time/date and the alarm.

The loop does nothing except check whether an alarm is running (alarm is stored in the RTC module itself) ; only when the rightmost button is pressed does it query the time from the RTC module, lit the screen and display the data.

Note: if the arduino is powered with a USB battery, the battery level does not work -- it will display 100% until the battery is dead, as it works by checking the voltage (which is regulated by the battery, so...).<br>
It does work when using for example a 3.7V NiMh battery and powering directly the arduino without using its usb port. One can also unsolder the voltage regulator to save some power, as long as the power supply is guaranteed not to be unstable.
