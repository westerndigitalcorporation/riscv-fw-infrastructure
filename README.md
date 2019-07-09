[comment]: [![N|Solid](https://content.riscv.org/wp-content/uploads/2018/09/unnamed.png)](https://nodesource.com/products/nsolid)

![N|Solid](http://riscv.net/wp-content/uploads/2015/01/riscv-logo-retina.png)

# WD RISC-V Firmware Package 
This repo is WD RISC-V Firmware package, holds:

  - WD-Firmware
  - GCC 8.2 Toolchain for RISC-V
  - Eclipse MCU


# Getting the sources
  If you dont have git lfs installed, please do the following steps:
  
    $ curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | sudo bash
    $ sudo apt-get install git-lfs
  
  Getting the repository 
    
    $  git clone https://github.com/westerndigitalcorporation/riscv-fw-infrastructure.git
  
[comment]:  [] (This repository uses submodules..... )
  
[comment]:  [] ($ git clone --recursiv https://bitbucket.wdc.com/scm/ctoriscvfwinfra/infra-riscv-fw.git)

# Code Convention 
See [code convention.htm](https://bitbucket.wdc.com/projects/CTORISCVFWINFRA/repos/infra-riscv-fw/browse)

# WD Firmware     
The “WD Firmware package” contains an Abstraction Layer (AL) on embedded small footprint real time operation systems (RTOS). The target is to provide homogenies API for the developer usage, so the kernel can be replaced for several different RTOS’s, without any need from the developer to change its application code.

Currently this AL supports FreeRTOS and ThreadX (only API's, without core) with a real running demo for FreeRTOS. 
The structure of WD Firmware package allows quick and easy integration for more RTOS’s, Platforms, boards and new firmware features. 

Coming soon: more FW features and more Platform supports 

The following README file describes how to setup a build environment for the WD RISC-V firmware with HiFive1 board; It will build an example application - FreeRTOS demo and describe how to download and debug it. The flow was verified with VMWare player v. 15 hosting Debian 9.6. This demo is based on the work done here https://github.com/sifive/freedom-e-sdk/tree/FreeRTOS/software/FreeRTOSv9.0.0

### Source tree structure 
```javascript
WD-Firmware
     |-board                                <-- supported boards
          |-hifive-1                        
          |-hifive-unleashed
     |-common                               <-- common source
     |-examples                             <-- examples source 
          |-build                           <-- examples build scripts
          |-ex-freertos                     <-- FreeRTOS example
     |-psp                                  <-- psp functionality
     |-rtos
          |-rtosal                          <-- rtos abstraction layer
          |-rtos_core                       <-- specific rtos source code
                |-freertos
                |-....
```

### Additional downloads
- From the repo unzip riscv-gnu-toolchain-8-linux.tar.xz to your designated directory for the tool chain
- From the repo unzip eclipse_mcu_2019_01.7z to your designated directory for the Eclipse MCU

      $ tar -xvf riscv-gnu-toolchain-8-linux.tar.xz -C [Toolchain-root]

      $ 7z x eclipse_mcu_2019_01.7z -o[Eclipse-MCU-root]

- Standard packages that are required can be installed by the following command:

      $ sudo apt-get install scons libftdi1-2 libmpfr4
    NOTE: If libmpfr4 can not be installed, in cases of newer versions '6', on the host machine, you can create a symbolic link to libmpfr.so.6
    
      $ sudo ln -s /usr/lib/x86_64-linux-gnu/libmpfr.so.6 /usr/lib/x86_64-linux-gnu/libmpfr.so.4 
- Download and install Java SE Runtime Environment

### Building for source
- Add the environment variable RISCV_TC_ROOT - set it to the *[Toolchain-root]* with the following command:

      $ sudo -H gedit /etc/environment

      RISCV_TC_ROOT=/path to [Toolchain-root]
      
- Reboot your machine for changes to take affect
- Launch Eclipse MCU - [Eclipse-MCU-root]/eclipse
- Import WD firmware code:
	- From 'Eclipse MCU' menu bar select *File* -> *Import*
	- In the Import window select *General* -> *Existing Projects into Workspace* -> *Next*
	- In the next Import window *Select root directory* -> *Browse*  and choose the infra-riscv-fw/WD-Firmware/ you've downloaded in 'Getting the firmware sources' section
	- Press *'Finish'* button
- From 'Eclipse MCU' menu bar select '*Project'* -> *'Build All'*

### Downloading & debugging the firmware image of HiFive1
- #### Setting up the hardware (taken from SiFive Freedom Studio Manual v1p6).
	- Connect to your HiFive1 debug interface and type "lsusb" to see if FT2232C is connected:

            $ lsusb
            Bus ... Device ... : ID 0403:6010 Future Technology Devices International, Ltd FT2232C Dual USB-UART/FIFO IC
            ...
            
	- Set the ___udev___ rules to allow the device to be accessed by the plugdev group

            $ sudo cp [WD-firmware-root]/WD-Firmware/board/hifive-1/99-openocd.rules  /etc/udev/rules.d/
            
	- Add and verify current user to ___plugdev___ group

            $ sudo usermod -a -G plugdev $USER
            $ groups
            ... plugdev ...
	- Power off/on Debian station
    
- #### Eclipse MCU configuration:
	- From the 'Eclipse MCU' menu bar press File->Properties->C/C++ Build->Settings, select the *Toolchain path* with bin folder: [Toolchain-root]/bin
    - From the Eclipse MCU menu bar select *'Run'* -> *'Debug Configuration'*
	- Create a New launch configuration for 'GDB OpenOCD Debugging', now on this configuration do the following:
        - Under *'Main tab'* Verify that the selected project is set to *WD-Firmware* if not, click the *'Browse...'* button and select it
        - Under *'Debugger tab'* verify on the *'OpenOCD Setup'* that the *'Actual executable'* is correct, should be: [Eclipse-MCU-root]/openocd/0.10.0-11-20190118-1134/bin/openocd
	    - Under *'Debugger tab'* under the *'OpenOCD Setup'* section, set the following to the *'Config options'* section 

                -f [WD-firmware-root]/WD-Firmware/board/hifive-1/wd-freedom-e300-hifive1.cfg 
            
	    - Under *'Debugger tab'* under the 'GDB Client Setup' section, set the following on the *'Commands'* window:

                set mem inaccessible-by-default off
                set arch riscv:rv32
                set remotetimeout 250             
            
	    - Under *'Startup tab'* uncheck the *'Pre-Run/Restart reset'* in the *'Run/Restart commands'* section
    
- Now that everything is set up, select from the *'Eclipse MCU'* menu bar *'Run'* -> *'Debug'*; this will download the WD firmware image and halt at the main() function.

### Adding new source modules

The folder WD-Firmware/examples/build/ contains a template file (SConscript_template) which can be used.

# GCC Releases
- #### RISCV GCC 8.2
	- Initial RISCV official 8.2 GCC release
- #### RISCV GCC 8.3
	- RISCV official 8.3 GCC release
	- GDB fix for skipping breakpoint at fist line after ASM lable
	- lto simple_object fix opening mode in windows (text->bin)
