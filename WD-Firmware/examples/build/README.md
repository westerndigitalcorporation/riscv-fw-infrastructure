# Build:
## Build from *IDE*
* if you haven't pick a demo yet then you should go to terminal and run the config script
  ```
  $ [WD-firmware-root]/WD-Firmware/demo/build
  $ ./config.h
  ```
* From 'Eclipse MCU' menu bar select 'Project' -> 'Build All'. Note that you can select which platform to build for.

## Build from *Terminal*
the building process is based on [Scons](https://scons.org/) system. therefore the build can be triggered from the console.
* Building the project from Condole:
  ```
  $ cd [WD-firmware-root]/WD-Firmware/demo/build
  ```

* if you haven't pick a demo yet then you should run the config command:
	```
	$ ./config.h
	```

* after the demo is set, then the build can be done by invoking the scons command:
  ```
  $ scons target="my_target"
  ```

* for more info regarding the Scons command line you can always use the command:
  ```
  $ scons -H
  ```