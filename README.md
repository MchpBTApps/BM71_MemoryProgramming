# Introduction

The BM70 and BM71 modules from Microchip are Bluetooth low energy modules designed for easy integration of Bluetooth low energy operation into a wide variety of applications. Microchip provides a Bluetooth low energy software stack programmed directly into the internal memory of the BM70/71 devices.

This repository provides details and example code on programming the BM71 to update firmware and memory configuration parameters. The example code provided covers two processes:
* Updating the module firmware with a host MCU via UART
* Changing memory configuration parameters of the module with a host MCU via UART

Both the process are similar for most part, except for the starting memory location, length of data to be written and the content being written. For this reason, the MPLabX standalone example provided is can used for both processes by making minor changes (uncommenting/commenting certain sections). The summary for each process is provided in the following sections.

## Device firmware update

Microchip releases periodic updates to the application firmware to include new features and bug fixes reported in previous firmware releases. These updates are provided with a release note along with the firmware files in the BM70/71 webpage. The process is update the firmware via UART is provided in the [BM70/71 device firmware update](https://www.microchip.com/content/dam/mchp/documents/WSG/ProductDocuments/UserGuides/50003134A.pdf) document in the BM70/71 webpage.

Microchip provides PC-based tools for programming the modules, which are available on the BM70/71 webpage. The details and the procedure for using these tools are provided in the [Microchip wiki](https://microchipdeveloper.com/ble:bm71-app-example-fw-upgrade) webpage.

For the rest of this document, it is assumed the user cannot use the PC Tool provided by Microchip to perform firmware update.

Note: The example code can be used to perform the firmware upgrade for both the RN487x and BM7x modules. Both modules have the same silicon with the firmware being different. 


### Creating the embedded array from firmware files

The firmware files needed for the firmware update process provided here can be downloaded 
from the [BM71 webpage](https://www.microchip.com/en-us/product/BM71) under the 'Embedded software' 
section. 

The MPLabX example in this repository needs these firmware files (which are in hex file format) 
to be converted to an embedded array format. For this, purpose, a python script is used. The python script, 
*BM7x_FileCreate.py*, takes in 4 mandatory input parameters. The syntax for executing the python script 
from command mode is shown below:

 *python BM7x_FileCreate.py [-h] [-f1 FILE1] [-f2 FILE2] [-f3 FILE3] [-f4 FILE4] [-o OUTPUT] [-d DEBUG]*

where:
- FILE1 : The .H01 firmware file 
- FILE2 : The .H02 firmware file 
- FILE3 : The .H03 firmware file 
- FILE4 : The .H04 firmware file 
- OUTPUT : The output filename
- DEBUG: The level of Debug messages to be displayed. This is set to 1 by default 
  (lowest number of debug messages).

#### Note that all the firmware files need to be in the same folder as the python script (being executed). 

Example:

*C:\>python BM7x_FileCreate_v3.py -f1 RN487x.H00 -f2 RN487x.H01 -f3 RN487x.H02 -f4 RN487x.H03 
-o RN487x_full.txt*

Output from above script:
- Firmware files to decode: 'RN487x.H00', 'RN487x.H01', 'RN487x.H02', 'RN487x.H03' debug level =1
- ---------Decoded RN487x.H00 ---------------------
- ---------Decoded RN487x.H01 ---------------------
- ---------Decoded RN487x.H02 ---------------------
- ---------Decoded RN487x.H03 ---------------------
- #######################################################################################
- ######### Decoded and merged hex files to an C array file: RN487x_full.txt ######
- #######################################################################################

The .txt file created by the python script is the embedded array used by the MPLab project to flash 
the firmware into the module.

### Executing the MPLabX code

The details of the setup used for the project in this repository are in the Setup section below. After 
the BM71 board is connected to the Explorer board, make sure the embedded array which contains the firmware 
array, is in the src folder of the MpLab folder.

To learn more about opening, writing and debugging MPLAb projects, please refer to the links below:

- [Open a project](http://microchipdeveloper.com/mplabx:open-a-project)
- [Program a project](http://microchipdeveloper.com/mplabx:build-release)
- [Debug a project](http://microchipdeveloper.com/mplabx:build-debug)
  
The MpLabX project can either execute the device firmware update process or write the memory configuration 
parameters. In order to execute the device firmware update, the user needs to uncomment the lines of code 
that define the parameters for device firmware update and comment out the lines of code relating to the 
memory configuration in **main.c**. The lines that need to be uncommented are shown below.
```
#define FLASH_TOTAL_SIZE 0x40000
#define FLASH_CHUNK_SIZE 128
#define FLASH_START_ADDRESS 0x00000000
#define FLASH_ERASE_LENGTH 0

const uint8_t dfu_image[] = {
    #include "RN487x_128.txt"
};
```
Compile and flash the project onto the PIC32. The device firmware update should successfully execute. 
In order to view the progress of device firmware update process, the user can connect to the Serial output 
of the Explorer board (Mini-USB port) and open a terminal emulator like Tera Term. The settings for the 
Tera Term are provided below:

- Baudrate - 115200.
- Data Bits - 8.
- Parity - None.
- Flow Control - None

 ## Memory parameters programming

Microchip releases periodic updates to the application firmware to include new features and bug fixes 
reported in previous firmware releases. These updates are provided with a release note along with the 
firmware files in the BM70/71 webpage. The process is update the firmware via UART is provided in the [BM70/71 device firmware update](https://www.microchip.com/content/dam/mchp/documents/WSG/ProductDocuments/UserGuides/50003134A.pdf) document in the BM70/71 webpage.

Microchip provides PC-based tools for programming the modules, which are available on the BM70/71 webpage. 
The details and the procedure for using these tools are provided in the [Microchip wiki](https://microchipdeveloper.com/ble:bm71-app-example-fw-upgrade) 
webpage.

For the rest of this document, it is assumed the user cannot use the PC Tool provided by Microchip to perform firmware update.

Note: The example code can be used to perform the firmware upgrade for both the RN487x and BM7x modules. Both modules have the same silicon with the firmware being different. 

###Using the UI GUI tool to customize memory parameters  

The firmware files needed for the firmware update process provided here can be downloaded 

from the [BM71 webpage](https://www.microchip.com/en-us/product/BM71) under the 'Embedded software' 

section. 



 *python BM7x_FileCreate.py [-h] [-f1 FILE1] [-f2 FILE2] [-f3 FILE3] [-f4 FILE4] [-o OUTPUT] [-d DEBUG]*

where:

- FILE1 : The .H01 firmware file 
- FILE2 : The .H02 firmware file 
- FILE3 : The .H03 firmware file 
- FILE4 : The .H04 firmware file 
- OUTPUT : The output filename
- DEBUG: The level of Debug messages to be displayed. This is set to 1 by default 
  (lowest number of debug messages).

#### Note that all the firmware files need to be in the same folder as the python script (being executed). 

Example:

*C:\>python BM7x_FileCreate_v3.py -f1 RN487x.H00 -f2 RN487x.H01 -f3 RN487x.H02 -f4 RN487x.H03 
-o RN487x_full.txt*

Output from above script:
- Firmware files to decode: 'RN487x.H00', 'RN487x.H01', 'RN487x.H02', 'RN487x.H03' debug level =1
- ---------Decoded RN487x.H00 ---------------------
- ---------Decoded RN487x.H01 ---------------------
- ---------Decoded RN487x.H02 ---------------------
- ---------Decoded RN487x.H03 ---------------------
- #######################################################################################
- ######### Decoded and merged hex files to an C array file: RN487x_full.txt ######
- #######################################################################################

The .txt file created by the python script is the embedded array used by the MPLab project to flash  
the firmware into the module.

### Executing the MPLabX code

The details of the setup used for the project in this repository are in the Setup section below. After 
the BM71 board is connected to the Explorer board, make sure the embedded array which contains the firmware 
array, is in the src folder of the MpLab folder.

To learn more about opening, writing and debugging MPLAb projects, please refer to the links below:
- [Open a project](http://microchipdeveloper.com/mplabx:open-a-project)
- [Program a project](http://microchipdeveloper.com/mplabx:build-release)
- [Debug a project](http://microchipdeveloper.com/mplabx:build-debug)

The MpLabX project can either execute the device firmware update process or write the memory configuration 
parameters. In order to execute the device firmware update, the user needs to uncomment the lines of code 
that define the parameters for device firmware update and comment out the lines of code relating to the 
memory configuration in **main.c**. The lines that need to be uncommented are shown below.

```

#define FLASH_TOTAL_SIZE 0x40000
#define FLASH_CHUNK_SIZE 128
#define FLASH_START_ADDRESS 0x00000000
#define FLASH_ERASE_LENGTH 0

const uint8_t dfu_image[] = {
    #include "RN487x_128.txt"
};
```

Compile and flash the project onto the PIC32. The device firmware update should successfully execute. 
In order to view the progress of device firmware update process, the user can connect to the Serial output 
of the Explorer board (Mini-USB port) and open a terminal emulator like Tera Term. The settings for the 
Tera Term are provided below:

- Baudrate - 115200.
- Data Bits - 8.
- Parity - None.
- Flow Control - None


## Setup

The following components are used for the MpLAb project:
- BM70 PICtail Plus (BM-70-PICTAIL)
- Explorer 16/32 Development Board (DM240001)
- PIC32MX570F512L

The following table shows the connections between the BM71 PICTail and the Explorer 8 board:

|BM71 pin|PICTail Interface pin (JP14)|Explorer 16/32 pin|
|--------|---------------------|------------------|
|P20|8|P57/RG2/SCL|
|RST_N|18|P97|
|HCI Tx|9|P9/RC4|
|HCI Rx|11|P20/POT/RB5|
|Vdd|26|Any 3.3V|
|Ground|28|Any Gnd|

