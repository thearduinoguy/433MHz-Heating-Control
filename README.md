# 433MHz Heating Control
Emulated on/off signals from a British Gas wireless thermostat to control my boiler

I captured the signals from the thermostat using an SDR dongle. Then recorded the 3 sets of pulses for the on and off signals. The resulting wav file was then fed into Audacity so I could view and stretch the pulses and figure out the binary code and timing.

Then this code was written to emulate the on/off signals using an Arduino Nano and a 433MHz radio transmitter module.

