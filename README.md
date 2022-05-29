# Documentation for setting up motor firmware with PlatformIO and mbed OS
This firmware is modified from Ben Katz's original motor firmware code which can be found [here](https://os.mbed.com/users/benkatz/code/HKC_MiniCheetah/).

## Setting up PlatformIO
PlatformIO is a compiler that is an extension built on top of the Visual Studio Code. Thus, to setup PlatformIO install Visual Studio Code first then search for the PlatformIO extension.

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/setting_up_1.png" width="75%" height="75%">

## Changing Flash Sector that stores motor settings

Due to the binary file that is being built taking up to sector 6 out the seven sector of the flash memory, the motor settings have to be stored in sector 7 instead to prevent overriding the motor firmware.

The line at which this is at is:
* Line 39

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/pref_1.png" width="75%" height="75%">

## Commenting out wait()

Due to the function being deprecated, using wait() gives an error when compiled in platformIO so wait_us() is used instead. 

### main.cpp file
The lines at which wait() are present are:
* Line 146

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/main_wait_1.png" width="75%" height="75%">

* Line 163

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/main_wait_2.png" width="75%" height="75%">

* Line 176

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/main_wait_3.png" width="75%" height="75%">

* Line 511 & Line 514

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/main_wait_4.png" width="75%" height="75%">

* Line 533

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/main_wait_5.png" width="75%" height="75%">


### foc.c file
The lines at which wait() are present are:
* Line 68

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/foc_wait_1.png" width="75%" height="75%">

### calibration.c file
The lines at which wait() are present are:
* Line 231

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/calibration_wait_1.png" width="75%" height="75%">

## Updating the platformio.ini file
### platformio.ini

Certain settings must be specified for the binary file to be build successfully. 
* The screenshot below shows what settings to add for the file. Currently, only mbed framework version 5.51304.0 builds a working firmware. 
```
[env:nucleo_f446re]
platform = ststm32
board = nucleo_f446re
framework = mbed
debug_tool = stlink
upload_protocol = stlink
platform_packages = framework-mbed @ ~5.51304.0
```

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/ini_setting.png" width="75%" height="75%">

* The latest framework of mbed does not build the binary file due to compatibility issue but the following are the mbed versions available for trying.
* The full list can be found at https://api.registry.platformio.org/v3/packages/platformio/tool/framework-mbed

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/mbed_version.jpg" width="75%" height="75%">

### compact.py
A bug from platformio side, `tostring` is deprecated so we need to change `tostring` to `tobytes` in `compact.py` file to build .bin file successfully.
* The file is located at C:\Users\{User_Name}\.platformio\packages\framework-mbed@6.51504.200716\platformio\package_deps\py3\intelhex
* Line 60, compact.py

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/compact_1.png" width="75%" height="75%">

* Line 92, compact.py

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/compact_2.png" width="75%" height="75%">


## Changing variable name "count"

For some reason, the `count` variable in main.cpp is raising a ambiguous referencing error. Thus, count has to be change from `count` to `count1` as a workaround for now.
The changes are at:
* Line 60

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/count_chng_1.png" width="75%" height="75%">

* Line 222, 229, 235, 237 

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/count_chng_2.png" width="75%" height="75%">

* Line 256, 261

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/count_chng_3.png" width="75%" height="75%">

* Line 296

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/count_chng_4.png" width="75%" height="75%">

## Changing openocd setting for ST Nucleo F4
After building, for the user to successfully flash in the firmware using the upload function in platform io the following setting need to be changed.
* C:\Users\{User_Name}\.platformio\packages\tool-openocd\scripts\board -> st_nucleo_f4.cfg

```
reset_config none separate
```

<img src="https://github.com/tikimonarch/Steadywin_Temp/tree/master/Pic/openocd_1.png" width="75%" height="75%">
