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
TM1637 8 segment 0.36INCH Digital Display Tube 4 digit LED module.   

![TM1637-1](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/59e95bf5-01bb-4c4a-9f96-4d36869be2e5)
![TM1637-2](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/e3430e3a-34ae-455a-8891-555ae89f2e0f)

# Installation
```
git clone https://github.com/nopnop2002/esp-idf-tm1637
cd esp-idf-tm1637/
idf.py menuconfig
idf.py flash
```

# Configuration   

![config-top](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/587b7ae6-0acd-4395-9672-5330b1b46a47)
![config-app](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/e338da80-66ee-423e-86f9-12bd109ce75a)


# Wirering

|TM1637||ESP32|ESP32-S2/S3|ESP32-C2/C3/C6||
|:-:|:-:|:-:|:-:|:-:|:-:|
|CLK|--|GPIO22|GPIO12|GPIO6|(*1)|
|DIO|--|GPIO21|GPIO11|GPIO5|(*1)|
|GND|--|GND|GND|GND||
|VCC|--|3.3V|3.3V|3.3V||

(*1)You can change it to any pin using menuconfig.   
