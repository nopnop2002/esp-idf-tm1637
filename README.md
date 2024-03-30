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
- Supported negative integer number display.   
- Supported negative real numbers display.   
- Supported 6 segments module.   

# Software requirements
ESP-IDF V4.4/V5.x.   
ESP-IDF V5.0 is required when using ESP32C2.   
ESP-IDF V5.1 is required when using ESP32C6.   

# Hardware requiments
TM1637 8 segment Digital Display Tube 4 digit LED module.   
There are several products with different segments and different sizes.   
- 0.36 Inch 4 Segments Product   
 8 segments with dots. Real numbers can be displayed.   
 Clock segments. Real numbers cannot be displayed.   
![0 36inchi](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/856f23ed-d198-4f68-b2a1-f085d59c0e11)

- 0.36 Inch 6 Segments Product   
 8 segments with dots. Real numbers can be displayed.   
![0 36inch-6segment](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/b3bf7b07-1f64-44ac-9ee7-a5fd159bb657)

- 0.56 Inchi 4 Segments Product   
 Clock segments. Real numbers cannot be displayed.   
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

![config-app-1](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/da9fad9b-f510-4a02-84a5-af44d588aa97)

Select Segments type   
![config-app-2](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/6a5b0c7c-32bd-4ede-b5ba-ff8bf9fba590)

Select number of Segments   
![config-app-3](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/f67fc28f-4b7d-4431-a61a-f9f41b453800)

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
