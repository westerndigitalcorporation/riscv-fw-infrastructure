# Demos:
This folder holds the demos supplied with the repository.
The demos has been set to build with [SCons](https://scons.org/) framework.
  
## Selecting a demo:
In order to select a demo to build you will need to use the *config.sh* script located in the build folder.
from terminal:
```sh
$ cd [WD-firmware-root]/WD-Firmware/demo/build
$ ./config.sh
```

This will prompt a list of all available demos in the current repository.
Pick the desired demo by entering the index value appear at the demo's name left. then build the project from eclipse normally.
`Note: Whenever you need to change the selected demo rerun the ./config.sh `
  
## Creating a new demo:
The demo library is located under the folder **[WD-firmware-root]/WD-Firmware/demo/build/demos**.
When running the *config.sh* script, the system will scan this folder looking for specific files in the following format:
`demo_`**name**`.py`

Therefore to add a new demo, the user should create a new python file inside the demo library, labeling it *demo_myName.py*
and this will make it visible to the scanner when running the *config.sh*.

### Inside the demo py:
The demo python file is constructed from a single class labeled **demo**, and has the following structure

```python
class demo(object):
    def __init__(self):
        self.strName = "myDemo"
        self.listSconscripts = []
```

`Note: the attributes strName and listSconscript are mandatory ones. you should keep it in the class`

* **strName** - *string*: will hold the name of the demo to be used throughout the build system
* **listSconscripts** - *list*: will hold the Sconscripts that the user want to include in the demo's build

Once done with setting up the demo, it will appear in the list shown to the user when running *config.sh* and the user can pick it up by entering it's index.

