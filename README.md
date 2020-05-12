[comment]: [![N|Solid](https://content.riscv.org/wp-content/uploads/2018/09/unnamed.png)](https://nodesource.com/products/nsolid)

![N|Solid](http://riscv.net/wp-content/uploads/2015/01/riscv-logo-retina.png)

# WD RISC-V Firmware Package 
This repo is WD RISC-V Firmware package, holds:

  - WD-Firmware
  - GCC 9.2.0 Toolchain for RISC-V
  - LLVM/Clang 10.0.0 Toolchain for RISC-V ***[along with GCC Binutils 2.32.51.20190122]***
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
The “WD Firmware package” constitutes an SDK FW. It contains Firmware applications and Processor Support Package (PSP) for various cores, alongside demos which support all features.
The following README file describes how to setup a build environment for the WD RISC-V firmware. It guides how to build the program, downloading it, and debugging it on the supported platforms and cores.
The FW-Infra was verified with VMWare player v. 15 hosting Debian 9.6. This demo was based on 


#### Current FW support:
- **RTOS-AL** 
    Abstraction Layer (AL) on embedded small footprint real time operation systems (RTOS). The target is to provide homogenies API for the developer usage, so the kernel can be replaced for several different RTOS’s, without any need from the developer to change its application code. 
    Currently this AL supports FreeRTOS and ThreadX (only API's, without core) with a real running demo for FreeRTOS. 
    The structure of WD Firmware package allows quick and easy integration for more RTOS’s, Platforms, boards and new firmware features. 
    Demos is based on HiFive FW example, by SiFIve. Cuurrently the reference is part of FreeRTOS maintenance 

- **Coming soon**: more FW features and more Platform supports 

#### Current Platform and Core support:
- **HiFive1** 
- **SweRV**    - running on *Nexys-A7 FPGA*
- **SweRVolf** - running on *Nexys-A7 FPGA* EH1 with full SoC
- **Whisper**  - ISS tool running on SweRVolf EH1


### Source tree structure 
```javascript
WD-Firmware
     ├── board                                <-- supported boards
          ├── hifive-1                        
          ├── ihfive-unleashed (not supported yet)
          ├── nexys_a7_eh1 (Support for SweRV eh1, also support SweRVolf)
          ├── whisper (Support for SweRV eh1)
     ├── common                               <-- common source
     ├── demo                                 <-- demos source 
          ├── build                           <-- example build scripts
          ├── demo_rtosal.c                   <-- Abstruction Layer (AL) demo on FreeRTOS
          ├── main.c                          <-- The main of all demos
          ├──  ....
     ├── psp                                  <-- psp functionality
     ├── rtos
          ├── rtosal                          <-- rtos abstraction layer
          ├── rtos_core                       <-- specific rtos source code
                ├── freertos
                ├── ....
```

### Additional downloads
	NOTE: The COMRV demo will work only with the LLVM toolchain, GCC is not supported
- #### Using GCC Toolchain
	- From the repo root folder unzip riscv-gnu-toolchain-debian.tar.gz to the ***WD-Firmware/demo/build/toolchain*** directory

      	  $ tar -xvf riscv-gnu-toolchain-debian.tar.gz -C ./WD-Firmware/demo/build/toolchain/

- #### Using LLVM Toolchain
	- From the repo root folder unzip riscv-llvm-toolchain-debian.tar.gz to the ***WD-Firmware/demo/build/toolchain*** directory
	
          $ tar -xvf riscv-llvm-toolchain-debian.tar.gz -C ./WD-Firmware/demo/build/toolchain/

- #### Other download
    - From the repo unzip eclipse_mcu_2019_01.7z to your designated directory for the Eclipse MCU

          $ 7z x eclipse_mcu_2019_12.7z -o[Eclipse-MCU-root]

    - Standard packages that are required can be installed by the following command:

          $ sudo apt-get install scons libftdi1-2 libmpfr4
    
        NOTE: If libmpfr4 can not be installed, in cases of newer versions '6', on the host machine, you can create a symbolic link to libmpfr.so.6
    
          $ sudo ln -s /usr/lib/x86_64-linux-gnu/libmpfr.so.6 /usr/lib/x86_64-linux-gnu/libmpfr.so.4 
    - Download and install Java SE Runtime Environment

    - For RISC-V OpenOCD, you will need the following depended libs: libusb-0.1, libusb-1.0-0-dev, libusb-dev

          $ sudo apt-get install libusb-0.1 libusb-1.0-0-dev libusb-dev

### Building for source
- #### Preparations 
    - Launch Eclipse MCU - [Eclipse-MCU-root]/eclipse
    - Import WD firmware code:
    	- From 'Eclipse MCU' menu bar select *File* -> *Import*
    	- In the Import window select *General* -> *Existing Projects into Workspace* -> *Next*
    	- In the next Import window *Select root directory* -> *Browse*  and choose the infra-riscv-fw/WD-Firmware/ you've downloaded in 'Getting the firmware sources' section
    	- Press *'Finish'* button
&nbsp;
- #### Build - compile and link 
    - You will need to choose a specific ***demo*** for building a full solution:
    
            From the eclipse terminal or console:
            $ cd [WD-firmware-root]/WD-Firmware/demo/build
            $ ./config.sh
            
            Then you will be asked to choose a demo.
            For more explanation on adding new demos please read the readme file on ***'/demos'***
            
    - From 'Eclipse MCU' menu bar select '*Project'* -> *'Build All'*. Note that you can select which platform to build for.
    - Since the building process use SCons build system, you can build via console/terminal. Please read the readme on ***’/build’*** 

### Platforms Downloading & debugging 
We provide several platforms to work with, please follow the instructions for the one you preferred.

- #### Setting up Hifive1 - FTDI over USB (taken from SiFive Freedom Studio Manual v1p6).
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
&nbsp;
- #### Setting up Nexys-A7 for SweRV - SweRVolf 

    SweRVolf is an fpga create by Olof Kindgren under CHIPS-Alliance
    If you wish to know more please use this link: [Cores-SweRVolf](https://github.com/chipsalliance/Cores-SweRVolf)
    
    - ***FPGA image file loading***: for loading the FPGA bit file, do the following steps:
    	- Copy the FPGA bit file /WD-Firmware/board/nexys_a7_eh1/***swervolf_0.bit*** to uSD device (locate it at the uSD root)
    	- Connect the uSD to the Nexys-A7 board (uSD slot is on board's bottom)
		- Set the following jumpers:  JP1 - connect JTAG & USB/SD pins.   JP2 - connect the 2 pins on 'SD' side
		- At power-on the FPGA bit file is loaded to the FPGA. LED 'Busy' should be ORANGE while flushing is done
		- Wait for ORANGE led to be off, once off the board is ready to be used 
&nbsp;
- #### Setting up Nexys-A7 for SweRV

    Since Nexys-A7 is an FPGA platform it need special handling...
    - ***Prerequisite***: 
    Following are prerequisite running SweRV core on Xilinx FPGA on Nexys-A7 board
        - To compile the RTL please follow the instruction at this link: [swerv_eh1_fpga](https://github.com/westerndigitalcorporation/swerv_eh1_fpga)
        - Our debugger uses the ***Olimex ARM-USB-Tiny-H*** Emulator with OpenOCD
        - Pin layout for Nexys Pmod JD header with Olimex:
        
                H4 = TDO
                H1 = nTRST
                G1 = TCK
                H2 = TDI
                G4 = TMS
                G2 = nRST

    - ***FPGA image file loading***: 
    for loading the FPGA bit file, do the following steps:
    	- Copy the FPGA bit file /WD-Firmware/board/nexys_a7_eh1/***eh1_reference_design.bit***
	   to uSD device (locate it at the uSD root)
    	- Connect the uSD to the Nexys-A7 board (uSD slot is on board's bottom)
		- Set the following jumpers:  JP1 - connect JTAG & USB/SD pins.   JP2 - connect the 2 pins on 'SD' side
		- At power-on the FPGA bit file is loaded to the FPGA. LED 'Busy' should be ORANGE while flushing is done.
		- Wait for ORANGE led to be off, once off the board is ready to be used
&nbsp;
- #### Setting up ISS (works as simulator for EH1)
    
    There is nothing to set for SweRV ISS, just select debugger luncher (following next)..
&nbsp; 
- #### Platforms Debug on Eclipse MCU Debug:
    - Select from the ***'Eclipse MCU'*** menu bar ***'Run' -> 'Debug Configurations...'***; 
    - Choose the platform you wish to run on, from **'left main windows'** menu
    - Current support:
        ```javascript
        - hifive1                              <-- HiFive Eval board
        - nexys_a7_eh1                         <-- Nexys A7 digilent FPGA board running SweRV EH1
        - nexys_a7_eh1_swerolf                 <-- Nexys A7 digilent FPGA board running SweRV EH1 
                                                   with full System on chip. 
                                                   From chipsalliance/Cores-SweRVolf
        - whisper_eh1_connect_and_debug        <-- SweRV ISS simulator 
        ```

### Adding new source modules

The folder WD-Firmware/demo/build/ contains a template file (SConscript_template) which can be used.
&nbsp;
# Supporting GCC Releases
- #### RISCV GCC 8.2
	- Initial RISCV official 8.2 GCC release
- #### RISCV GCC 8.3
	- RISCV official 8.3 GCC release
	- GDB fix for skipping breakpoint at fist line after ASM lable
	- lto simple_object fix opening mode in windows (text->bin)
- #### RISCV GCC 9.2
	- RISCV official 9.2 GCC release
	- WD Code density improvement and optimization patches


# Supporting LLVM Releases
- #### RISCV LLVM/Clang 10.0.0
	- Initial LLVM/Clang official 10.0.0 release
	- ComRV support modules
	- GCC Binutils-gdb 2.32.51.20190122 supporting ComRV
