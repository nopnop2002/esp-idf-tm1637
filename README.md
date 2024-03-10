# esp-idf-tm1637
TM1637 Driver for esp-idf.   
I based it on [this](https://github.com/petrows/esp-32-tm1637).   

![tm1637-ip](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/15cb623d-2298-4344-a9c7-f573cb0255cd)
![tm1637-play](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/bb98913e-f5e5-4f17-8d12-4cd9f16518f7)
![tm1637-stop](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/b1928da8-c461-4772-8d33-8f82175c5f54)

# Changes from the original
- Applied to ESP-IDF Ver5.   
- Added display of ascii characters.   
- Added text scroll display function.   
- Added text animation display function.   

# Software requirements
ESP-IDF V4.4/V5.x.   
ESP-IDF V5.0 is required when using ESP32C2.   
ESP-IDF V5.1 is required when using ESP32C6.   

# Hardware requiments
TM1637 8 segment Digital Display Tube 4 digit LED module.   
There are several products with different segments and different sizes.   
- 0.36 Inch Product   
 8 segments with dots + 8 segments with dots + 8 segments with dots + 8 segments with dots   
![product-1-0 36](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/4d3e4ba0-5248-4e45-8dd1-c1a80ba501f8)

- 0.36 Inch Product   
 7 segments + 8 segments with colon + 7 segments + 7 segments   
![product-2-0 36](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/ac397e9c-815d-479f-a4a0-2676cca3218b)

- 0.56 Inchi Product   
 7 segments + 8 segments with colon + 7 segments + 7 segments   
 The dots do not light up   
![0 56-1](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/54afb3f0-c6dc-46a4-9b77-6809cd70e2e8)

# Installation
```
git clone https://github.com/nopnop2002/esp-idf-tm1637
cd esp-idf-tm1637/
idf.py menuconfig
idf.py flash
```

# Configuration   

![config-top](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/587b7ae6-0acd-4395-9672-5330b1b46a47)
![config-app](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/db480fbd-8e0d-4cb2-9197-c9933ac04338)


# Wirering

|TM1637||ESP32|ESP32-S2/S3|ESP32-C2/C3/C6||
|:-:|:-:|:-:|:-:|:-:|:-:|
|CLK|--|GPIO22|GPIO12|GPIO6|(*1)|
|DIO|--|GPIO21|GPIO11|GPIO5|(*1)|
|GND|--|GND|GND|GND||
|VCC|--|3.3V|3.3V|3.3V||

(*1)   
The TM1637's interface is similar to I2C, but it is not I2C.   
You can change it to any pin using menuconfig.   
