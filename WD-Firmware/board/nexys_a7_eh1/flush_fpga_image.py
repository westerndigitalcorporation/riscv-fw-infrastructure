"""
:Author: Ofer Shinaar
:Email: Ofer.shinaar@wdc.com
:Version: 0.0.1
:Update date:
    * *Oct 2019* - Originally created

Flush Nexys A7 fpga image - SweRV

"""

import os
import sys
from sys import platform

STR_VIVADO_PATH = "VIVADO_PATH"

STR_TCL_FILE = "swerv_flush_fpga.tcl"
STR_VIVADO_EXE = "vivado"
STR_VIVADO_FLAGS = "-mode batch -source" 

def HelpMessageforSetEnv():
    print platform
    if platform == "linux" or platform == "linux2":
        print "Please do: export VIVADO_PATH=<your path to vivado exe>"
    elif platform == "win32":
        print "Please do: set VIVADO_PATH=<your path to vivado exe>"
        
    

def main():
    for arg in sys.argv[1:]:
	print arg
	exit(0)

    if STR_VIVADO_PATH not in os.environ.keys():
        print "Please update environment variable VIVADO_PATH to the Vivado excecution file with path"
        HelpMessageforSetEnv()
        exit(0)
        
    elif os.environ[STR_VIVADO_PATH] == '':
        print "Please update environment variable VIVADO_PATH to the Vivado excecution file with path"
        HelpMessageforSetEnv()
        exit(0)
        
    else:
        VIVADO_EXE = os.path.join(os.environ[STR_VIVADO_PATH], STR_VIVADO_EXE)
    
    
    
    exeCmd = "%s %s %s" %(VIVADO_EXE, STR_VIVADO_FLAGS, STR_TCL_FILE)
    
    print "running %s" %exeCmd
    os.system(exeCmd)


if __name__ == "__main__":
    main()

	
