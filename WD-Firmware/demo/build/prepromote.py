import os
import re
import sys
import shlex
import json
import glob
import time
import logging
import importlib
import threading
import subprocess
import signal
import csv
import argparse
import Queue
import traceback
from subprocess import check_output
from copy import deepcopy
import multiprocessing
try:
    import serial
except ImportError as e:
    ret = os.system("python -m pip install pyserial")
    if ret:
        print "could not install pyserial, please install it manually then try again"
        exit(1)
    import serial

#TODO get is as a python input
INT_DEMO_TO_IN_SECONDS = 7*60
INT_PLATFORM_EXE_DELAY_IN_SECS = 2
INT_TOOLCHAIN_INDEX = 0
INT_PLATFORM_NAME_INDEX = 1
INT_TARGET_NAME_INDEX = 2
INT_DEMO_NAME_INDEX = 3
INT_RESULT_INDEX = 4
INT_CSV_FILE_ARG_INDEX = 0
INT_DEMO_LIST_ARG_INDEX = 1

STR_PASSED = "passed"
STR_FAILED = "failed"
STR_MODULE_PATTERN = "demos.demo_%s"
STR_GDB_CONF_PATTERN = "swervolf_nexys_%s_debug.cfg"
STR_RUN_CONF_PATTERN = "swervolf_nexys_%s_debug.cfg"
STR_TEST_STARTED = ".... start"
STR_TEST_ENDED = ".... end"
STR_TEST_ERROR = ".... error"
STR_END_OF_LOG = "end-of-log"
STR_LISTENER_ABORTED = "listener-abort"
STR_ROOT_DIR = os.getcwd()
STR_WS_FOLDER = STR_ROOT_DIR.replace("demo/build", "")
STR_BOARDS_FOLDERS = os.path.join(STR_WS_FOLDER, "board")
STR_OUTPUT_FOLDER = os.path.join(STR_WS_FOLDER, "demo/build/output")
if not os.path.isdir(STR_OUTPUT_FOLDER):
    os.mkdir(STR_OUTPUT_FOLDER)

STR_CI_FOLDER = os.path.join(STR_OUTPUT_FOLDER, "prepromote")
if not os.path.isdir(STR_CI_FOLDER):
    os.mkdir(STR_CI_FOLDER)

STR_PTS_OUTFILE = os.path.join(STR_CI_FOLDER, "pts.txt")
STR_DEMOS_FOLDER = os.path.join(STR_WS_FOLDER, "demo/build/demos")

STR_BUILD_CMD = "scons target=%s"
STR_CLEAN_CMD = "%s -c" % STR_BUILD_CMD

STR_BIN_EXT = ".elf"

strLogFile = os.path.join(STR_CI_FOLDER, "prepromote.log")
if os.path.isfile(strLogFile):
    os.unlink(strLogFile)
log = logging.getLogger("prepromote")
log.setLevel(logging.DEBUG)
#log.setLevel(logging.INFO)
# create file handler which logs even debug messages
fh = logging.FileHandler(strLogFile)
# create console handler with a higher log level
ch = logging.StreamHandler()
# create formatter and add it to the handlers
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
fh.setFormatter(formatter)
ch.setFormatter(formatter)
# add the handlers to the logger
log.addHandler(fh)
log.addHandler(ch)
log.info("testing log")

# this class is responsible for managing the prepromote.json
class clsData(object):

    def __init__(self, strDemosList):
        try:
            self.dictResults = {}
            self.fnParseUserDemosList(strDemosList)
            dictJSONConfig = self.fnGetJsonFile()
            for key, val in dictJSONConfig.iteritems():
                setattr(self, key, val)
        except Exception as e:
            log.debug("clsData __init__")
            raise e

    # Load the .json file and return its content
    def fnGetJsonFile(self):
        try:
            f = open("prepromote.json", 'r')
            dictData = self.fnByteify(json.load(f))
            f.close()
        except Exception as e:
            log.debug("clsData fnGetJsonFile")
            raise e
        return dictData

    def fnByteify(self, objData):
        if isinstance(objData, dict):
            return {self.fnByteify(key): self.fnByteify(value)
                    for key, value in objData.iteritems()}
        elif isinstance(objData, list):
            return [self.fnByteify(element) for element in objData]
        elif isinstance(objData, unicode):
            return str(objData)
        else:
            return objData

    # this function is responsible  for renaming demos that shouldn't be executed
    # function args:
    # strUserDemosList - list of demos to execute
    def fnParseUserDemosList(self, strUserDemosList):
        try:
            # check if we need to filter demos - end user provided a demo list
            if strUserDemosList != None:
                strUserDemosList = re.findall(r"[\w']+", strUserDemosList)
                listExistingDemos = []
                # build a list of all existing demos
                for strFilename in os.listdir(STR_DEMOS_FOLDER):
                    if strFilename.startswith("demo") and strFilename.endswith(".py"):
                          listExistingDemos.append(strFilename)
                # remove from the list all demos the end user wishes to execute
                listExistingDemosDuplicated = listExistingDemos[:]
                for strDemoName in strUserDemosList:
                    for strName in listExistingDemosDuplicated:
                        if strDemoName in strName:
                            listExistingDemos.pop(listExistingDemos.index(strName))
                # the list now contains all demos that don't need to be executed - 
                # rename them so they won't be included
                for strDemoName in listExistingDemos:
                    strFileName = os.path.join(STR_DEMOS_FOLDER, strDemoName)
                    res = os.system("mv %s %s-" % (strFileName, strFileName))
                    if res != 0:
                        log.error("failed to rename demos")
                        exit(1)
        except Exception as e:
            log.debug("clsData fnParseUserDemosList")
            raise e

'''
class clsReadPts(object):

    def __init__(self, strBoard):
        global strTty
        self.strBoard = strBoard
        self.boolTestDone = False
        open(STR_PTS_OUTFILE, 'w').close()
        if strTty == "":
            stream = os.popen('tty')
            output = stream.read()
            stream.close()
            strTty = output.rstrip()

    def fnStart(self):
        global strTty
        t = threading.Thread(target=self.fnStartListening, args=(strTty, ""))
        t.start()

    def fnStartListening(self, strPTSIndex, str):
        f = open(STR_PTS_OUTFILE, "r")
        while self.boolTestDone != True:
            line = f.readline()
            if line == "":
                time.sleep(2)
                continue
            log.debug("pts %s" % line)
            if STR_TEST_STARTED in line:
                 log.info("%s demo start" % strPTSIndex)
            elif STR_TEST_ENDED in line or STR_TEST_ERROR in line:
                f.close()
                log.info("%s demo end" % strPTSIndex)
                self.boolTestDone = True
'''
# a simple class for queue of exceptions
class clsExceptionQueue(Queue.Queue):

    # get a snapshot of the queue contents
    def __fnSnapshot(self):
        with self.mutex:
            return list(self.queue)

    # check if the queue isn't empty
    # functions args:
    # boolRaiseException - True, if the queue isn't empty, raise the exception 
    #                      False, if the queue isn't empty, don't raise an exception 
    def fnQueryException(self, boolRaiseException = True):
        #raise Exception
        # get a snapshot of the queue contents
        listExceptions = self.__fnSnapshot()
        if len(listExceptions):
            if boolRaiseException:
                # get an item from the queue
                e = self.get()
                # we can't re-raise KeyboardInterrupt so we raise a new exception
                if e == KeyboardInterrupt:
                    raise Exception("KeyboardInterrupt user aborted")
                else:
                    raise e
            return True
        return False

# a listener class responsible for parsing data from an output source 
# an output example can be a file, usb, pst, etc.
# the general purpose of the class is to capture from the output the 'start' and 'end'/'error'
# that each executing demo prints  
#                 clsListener
#                   /     \
#                  /       \
#                 /         \
#    clsSerialListener   clsFileListener
class clsListener(object):

    # strLog - log file path
    # boolStoreResults - should we log or not
    def __init__(self, strLog, boolStoreResults = True):
        try:
            # there can be several listeners - this holds the index of the actual listener
            self.strConnectedListenerIndex = ""
            # marc if the listener completed 
            self.boolListenerDone = False
            # listener log file 
            self.strLog = strLog
            # list of listeners
            self.listListeners = []
            # should we store the results - in case of a file listener there is no need to store 
            # the captured data as it is already stored in a file
            self.boolStoreResults = boolStoreResults
            # number of listeners
            self.NumOfListeners = 0
            # exception queue
            self.queue = clsExceptionQueue()
        except Exception as e:
            log.debug("exception clsListener __init__")
            raise e

    # Each listener runs in it's own thread, this function starts the thread
    # Function parameters: these parameters are passed directly to the function thread 
    # strListenerIndex - string representing the listener index
    # listenerHandle - points to the listener e.g. file, usb, etc.
    def fnStart(self, strListenerIndex, listenerHandle):
        try:
            # Create the thread
            t = threading.Thread(target=self.fnStartListening, args=(strListenerIndex, listenerHandle))
            # Start the thread
            t.start()
        except Exception as e:
            log.debug("exception clsListener fnStart")
            raise e

    # this is the listener function each thread executes
    # Function parameters:  
    # strListenerIndex - string representing the listener index
    # listenerHandle - points to the listener e.g. file, usb, etc.
    def fnStartListening(self, strListenerIndex, listenerHandle):
        # listener list to hold the data
        listData = []
        data = None
        exceptionSaved = None
        self.NumOfListeners += 1
        log.debug("clsListener fnStartListening enter %s" % strListenerIndex)
        try:
            # Keep listening as long as not completed yet 
            while self.boolListenerDone == False:
                 try:
                     # Read one line
                     data = listenerHandle.readline().strip()
                 except Exception as e:
                     # Make sure we only log once the exception in case  
                     # it occurs several times
                     if exceptionSaved == None or exceptionSaved != e:
                        log.debug("%s exception %s" % (strListenerIndex, str(e)))
                        exceptionSaved = e
                     pass
                 # in some cases we may get \x00 which will cause the script to get stuck
                 # so if we find it we ignore it
                 if not data or data.find("\x00") != -1:
                     continue
                 # log the data we got
                 log.debug("%s read: %s" % (strListenerIndex, data))
                 # add the data to the list
                 listData.append(data)
                 # capture start of demo
                 if STR_TEST_STARTED in data:
                     # save the listener string index for later use 
                     self.strConnectedListenerIndex = strListenerIndex
                     log.info("%s demo start" % strListenerIndex)
                 # capture end of demo only by the selected listener
                 elif (self.strConnectedListenerIndex == strListenerIndex) and (STR_TEST_ENDED in data or STR_TEST_ERROR in data):
                     # mark we are done
                     self.boolListenerDone = True
                     #log.debug(listData)
                     log.info("%s demo end" % strListenerIndex)
                 # we still have other USB capturing 'end' from old running test on h51 which is still running
                 elif self.strConnectedListenerIndex != "" and self.strConnectedListenerIndex != strListenerIndex and (STR_TEST_ENDED in data or STR_TEST_ERROR in data):
                     listData = []
                     break
                 # listener got aborted
                 elif STR_LISTENER_ABORTED in data:
                     # mark we are done
                     self.boolListenerDone = True
                     log.info("%s demo aborted" % strListenerIndex)
            # log the captured data if got any and if were asked to be stored
            if self.boolStoreResults and len(listData) > 0:
                log.debug("clsListener fnStartListening save to file")
                f = open(self.strLog, "a")
                f.writelines("\n".join(listData))
                f.close()
            # listener done running so we can reduce the global listeners counter
            self.NumOfListeners -= 1
            log.debug("clsListener fnStartListening exit %s" % strListenerIndex)
        except Exception as e:
            # save all exceptions in a queue - to exit in a clean way w/o interrupting other listeners 
            self.queue.put(e)
            # listener done running so we can reduce the global listeners counter
            self.NumOfListeners -= 1
            log.debug("exception clsListener fnStartListening")
            pass

    # this function is responsible for waiting until all listeners have
    # completed executing the thread function
    def fnWaitForCompletion(self, boolForceAbort = False):
        try:
            # loop as long as the listeners counter didn't get to 0
            while self.NumOfListeners != 0:
                time.sleep(1)
        except Exception as e:
            log.debug("exception clsListener fnWaitForCompletion")
            raise e
    
    # allow from the outide to query if an exception occurred
    def fnQueryException(self, boolRaiseException = True):
        return self.queue.fnQueryException(boolRaiseException)

# this is a file listener class
#                 clsListener
#                   /     \
#                  /       \
#                 /         \
#    clsSerialListener   clsFileListener
class clsFileListener(clsListener):

    def __init__(self, strLog):
        try:
            # init parent object
            super(clsFileListener, self).__init__(strLog, False)
            self.listListeners.append(strLog)
            self.fileHandler = None
            # make sure the log file is created and empty
            open(strLog, 'w').close()
        except Exception as e:
            log.debug("exception clsFileListener __init__")
            raise e

    # start the listener thread
    def fnStart(self, strListenerIndex = None, listenerHandle = None):
        try:
            # open the file to listen to
            self.fileHandler = open(self.listListeners[0], "r")
            # create and start the tread using parent interface
            super(clsFileListener, self).fnStart("file listener", self.fileHandler)
        except Exception as e:
            log.debug("exception clsFileListener fnStart")
            raise e

    # listener thread - here we only call parent interface and close the file once 
    # thread completes 
    def fnStartListening(self, strListenerIndex, listenerHandle):
        try:
            # parent thread listener function
            super(clsFileListener,  self).fnStartListening(strListenerIndex, listenerHandle)
            # now that listener is completed, close the file
            self.fileHandler.close()
        except Exception as e:
            log.debug("exception clsFileListener fnStartListening")
            raise e

    # wait for listener completion
    def fnWaitForCompletion(self, boolForceAbort):
        try:
            # if we need to abort
            if boolForceAbort:
                log.debug("clsFileListener fnWaitForCompletion before")
                fileHandler = open(self.listListeners[0], "a")
                #inject the file that listener needs to abort
                fileHandler.write(STR_LISTENER_ABORTED)
                fileHandler.close()
                log.debug("clsFileListener fnWaitForCompletion after")
            # call parent function
            super(clsFileListener,  self).fnWaitForCompletion()
        except Exception as e:
            log.debug("exception clsFileListener fnWaitForCompletion")
            raise e


# this is a serial listener class
#                 clsListener
#                   /     \
#                  /       \
#                 /         \
#    clsSerialListener   clsFileListener
class clsSerialListener(clsListener):

    def __init__(self, strLog):
        try:
            # init parent class
            super(clsSerialListener, self).__init__(strLog)
            # get the list of connected USBs
            self.listListeners = glob.glob("/dev/ttyUSB*")
            # if no USB found - exit
            if not self.listListeners:
                log.error("Error finding usb drivers")
                exit(1)
        except Exception as e:
            log.debug("exception clsSerialListener __init__")
            raise e

    # start the listener treads - depending on the number of connected USBs
    def fnStart(self, strListenerIndex = None, listenerHandle = None):
        try:
            # for each connected USB start its own listener tread
            # thread function is the one in clsListener
            for strUsb in self.listListeners:
                # create the string index representing the specific listener
                strUSBIndex = strUsb.replace("/dev/tty", "")
                # open the serial connextion
                serialHandle = serial.Serial(strUsb, 115200)
                # a time out must be set otherwise the specific listener may block forever
                # as it isn't executing a demo (other usb listener is executing)
                # each one second, the thread will check if it got aborted to avoid the blocking 
                serialHandle.timeout = 1
                # start the tread
                super(clsSerialListener, self).fnStart(strUSBIndex, serialHandle)
        except Exception as e:
            log.debug("exception clsSerialListener fnStart")
            raise e

    # wait for listener completion
    def fnWaitForCompletion(self, boolForceAbort):
        try:
            # if we force abort
            if boolForceAbort:
                # set abort flag
                self.boolListenerDone = True
                log.debug("clsSerialListener fnWaitForCompletion")
            # wait for completion
            super(clsSerialListener,  self).fnWaitForCompletion()
        except Exception as e:
            log.debug("exception clsSerialListener fnWaitForCompletion")
            raise e

# factory class to create listeners depending on the interface type
class clsListenerFactory(object):

    def fnCreateListener(self, strInterfaceName, stLogFileName):
        try:
            # create serial listener
            if strInterfaceName == "openocd":
                return clsSerialListener(stLogFileName)
            # create file listener
            elif strInterfaceName == "whisper":
                return clsFileListener(stLogFileName)
            # unsupported interface
            else:
                log.error("Error: invalid interface name: %s" % strInterfaceName)
                exit(1)
        except Exception as e:
            log.debug("exception clsListenerFactory fnCreateListener")
            raise e

# GDB class
class clsGdb(object):

    # parameters:
    # dictConfig - GDB session parameters
    # sessionNumber - gdb session number (each session has its own session number)
    # strTimeStamp - to be used as a uniqe log file name
    def __init__(self, dictConfig, sessionNumber, strTimeStamp):
        try:
            # full path of GDB including the executable GDB file name taken from the prepromote.json file 
            self.strCMD = os.path.join(dictConfig["gdb"]["location"].format(WS=STR_WS_FOLDER), dictConfig["gdb"]["command"])
            # object process for the GDB session
            self.objProcess = object
            # hold the GDB session parameters
            self.dictConfig = dictConfig
            # session number
            self.strSessionNumber = str(sessionNumber)
            # GDB session log file
            self.strLog = os.path.join(STR_CI_FOLDER, "%s_%s_gdb_%s.log" % (strTimeStamp, self.dictConfig["board"],self.strSessionNumber))
        except Exception as e:
            log.debug("exception clsGdb __init__")
            raise e

    # Each GDB session runs in its own process -this function creates and runs the process 
    def fnStart(self):
        try:
            # open the GDB session's log file 
            raw_logfile = open(self.strLog, "wb")
            # the process command line
            strCommand = "%s %s -x %s" % (self.strCMD, self.dictConfig["elf"], self.fnCreateGdbCommandFile())
            # start the session
            self.objProcess = subprocess.Popen(shlex.split(strCommand), stdin=subprocess.PIPE, stdout=raw_logfile,
                                               stderr=raw_logfile, shell=False, cwd=self.dictConfig["board_wd"])
        except Exception as e:
            log.debug("exception clsGdb fnStart")
            raise e

    # create the GDB command file to used
    # the created file will be used by fnStart then starting the GDB process
    def fnCreateGdbCommandFile(self):
        try:
            # hold the list of GDB commands
            listData = []
            # place in the list all GDB commands from the prepromote.json file
            for gdbCmd in self.dictConfig["gdb_cmds_" + self.strSessionNumber]:
                # before inserting the specific GDB command into the list
                # replace WD with the actual true path
                listData.append(gdbCmd.format(WS=STR_WS_FOLDER))
            strData = "\n".join(cmd for cmd in listData if cmd)
            # GDB commands file name
            strCmdFile = os.path.join(STR_CI_FOLDER, "%s_gdb_cmd_%s" % (self.dictConfig["board"], self.strSessionNumber))
            # Write all the commands to the file
            f = open(strCmdFile, "w")
            f.write(strData)
            f.close()
        except Exception as e:
            log.debug("exception clsGdb fnCreateGdbCommandFile")
            raise e
        return strCmdFile

    # this function terminates the GDB session 
    def fnTerminateProcess(self):
        try:
            log.debug("clsGdb fnTerminateProcess(): enter %s" % self.strSessionNumber)
            # if the process still exist
            if self.objProcess.poll() == None:
                # terminate GDB process
                self.objProcess.terminate()
                # wait for process termination
                self.objProcess.wait()
                # if the process still exist, wait for 1 second and kill it
                # this case should never happen
                while self.objProcess.poll() == None:
                    log.debug("clsGdb fnTerminateProcess(): self.objProcess.poll() == None")
                    time.sleep(1)
                    self.objProcess.kill()
            log.debug("clsGdb fnTerminateProcess(): exit %s" % self.strSessionNumber)
        except Exception as e:
            log.debug("exception clsGdb fnTerminateProcess")
            raise e

# this class is responsible for executing a demo on a specific platform
# it opens an openocd/whisper process, GDB process and a listener thread/s
class clsPlatform(object):

    def __init__(self, dictConfig):
        try:
            self.dictConfig = dictConfig
            self.objProcess = object
            # timestamp used to create a unique file name
            self.strTimestamp = str(time.time())
            log.info(self.strTimestamp)
            # openocd/whisper log file name
            self.strLog = os.path.join(STR_CI_FOLDER, "%s_%s.log" % (self.strTimestamp, self.dictConfig["board"]))
            # holds the list of GDB sessions
            self.objGdb = []
            self.listenerObj = None
            self.queue = clsExceptionQueue()
        except Exception as e:
            log.debug("exception clsPlatform __init__")
            raise e

    # capture end user keyboard interrupt
    def fnHandleKyboardInterrupt(self, signalnum, handler):
        self.queue.put(KeyboardInterrupt)

    # run a specific demo
    def fnRun(self):
        try:
            # register our specific keyboard interrupt function so we are able
            # to abort in a clean manner 
            fnOldHandler = signal.signal(signal.SIGINT, self.fnHandleKyboardInterrupt)
            # perform targer flush - in case of whisper it won't do anything
            self.fnFlush()
            # create a listener according to the interface (whisper/openocd)
            self.listenerObj = clsListenerFactory().fnCreateListener(self.dictConfig["interface_name"], self.strLog)
            # get the number of required GDB sessions
            gdbSessions = int(self.dictConfig["gdb_instances"])
            # start the listener/s
            self.listenerObj.fnStart()
            # start the whisper/openocd
            self.fnStart()
            # start the GDB sessions
            for i in range(gdbSessions):
                # we must have a delta between each session start 
                time.sleep(2)
                self.objGdb.append(clsGdb(self.dictConfig, i+1, self.strTimestamp))
                # start the GDB session
                self.objGdb[i].fnStart()
            # used to catch a timeout
            intAccumulatedRuntime = 0
            log.debug("clsPlatform fnRun(): while True: enter")
            # at this point GDB and openocd/whisper are running and the
            # listener is capturing the demo output
            # we need now to wait for the demo completion or timeout 
            while True:
                # allow the other processes to run 
                time.sleep(INT_PLATFORM_EXE_DELAY_IN_SECS)
                # check if we got an exception
                self.fnCheckForException()
                # accumulated time
                intAccumulatedRuntime += INT_PLATFORM_EXE_DELAY_IN_SECS
                # check if demo is completed or we reached a timeout
                if self.listenerObj.boolListenerDone or intAccumulatedRuntime == INT_DEMO_TO_IN_SECONDS:
                    # terminate all running processes
                    self.fnTerminateCurrentExecution(intAccumulatedRuntime == INT_DEMO_TO_IN_SECONDS)
                    # we can now quit the run
                    break
            log.debug("clsPlatform fnRun(): while True: exit")
            signal.signal(signal.SIGINT, fnOldHandler)
        except Exception as e:
            log.debug("exception clsPlatform fnRun")
            raise e
        return self.strLog

    # check if any process raised an exception
    def fnCheckForException(self):
        self.queue.fnQueryException();
        self.listenerObj.fnQueryException();

    # start openocd/whisper process 
    def fnStart(self):
        try:
            # open the openocd/whisper log file
            raw_logfile = open(self.strLog, "wb")
            listIFCMD = shlex.split(
                "%s" % self.dictConfig["execute"].format(WS=STR_WS_FOLDER))
            # start the openocd/whisper process
            self.objProcess = subprocess.Popen(listIFCMD, stdin=subprocess.PIPE, stdout=raw_logfile,
                                               stderr=raw_logfile, shell=False, cwd=self.dictConfig["board_wd"])
        except Exception as e:
            log.debug("exception clsPlatform fnStart")
            raise e
    
    # perform flush
    def fnFlush(self):
        try:
            # if flush is required
            if self.dictConfig["flush"] != "":
                log.info("Flushing %s" % self.dictConfig["board"])
                # flush command
                listCommand = shlex.split("%s" % (self.dictConfig["flush"].format(WS=STR_WS_FOLDER)))
                # start the flush process
                proc = subprocess.Popen(listCommand, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                        shell=False, cwd=self.dictConfig["board_wd"])
                # wait for the flush to complete
                data, err = proc.communicate()
                # did we get an error
                if "->DONE!!!!!!!" not in data + err:
                    log.error("error in fulshing device %s" % self.dictConfig["board"])
                    log.debug(data)
                    log.debug(err)
                    exit(1)
                log.info("Flushing completed")
        except Exception as e:
            log.debug("exception clsPlatform fnFlush")
            raise e

    # terminate the openocd/whisper 
    def fnTerminateProcess(self):
        try:
            # if process is still alive
            if self.objProcess.poll() == None:
                # terminate it
                self.objProcess.terminate()
                # wait for termination to complete
                self.objProcess.wait()
                # if the process still exist
                # this case should never happen
                while self.objProcess.poll() == None:
                    log.debug("After termination openocd Still running")
        except Exception as e:
            log.debug("exception clsPlatform fnTerminateProcess")
            raise e

    # terminate all running processes and listeners 
    def fnTerminateCurrentExecution(self, boolForceTermination):
        # terminate openocd/whisper
        self.fnTerminateProcess()
        # terminate GDB sessions
        for i in range(len(self.objGdb)):
            self.objGdb[i].fnTerminateProcess()
        # Terminate listeners 
        self.listenerObj.fnWaitForCompletion(boolForceTermination)
        self.objGdb = []


# this class is responsible for executing all demos on all interfaces
# it shall prepare all parameters for the specific demo to run and use  
# clsPlatform to execute it
class clsPrepromote(object):

    def __init__(self, fileExpCsv):
        try:
            self.objConfig = object
            self.strBuildFolder = ""
            self.dictDemos = {}
            self.dictResults = {}
            self.dictBoards = {}
            self.listBoardsFolders = os.listdir(STR_BOARDS_FOLDERS)
            self.fnSetup()
            self.fnGetDemos()
            self.listDemoResults = []
            self.boolDemosFail = False
            self.listDemoFailedResults = []
            self.platformObj = None
            self.fileExpectedResultsCsv = fileExpCsv
            self.strExpectedHeaderLine = self.fnGetListOfTargetsToRun()
        except Exception as e:
            log.debug("exception clsPrepromote __init__")
            raise e

    def fnSetup(self):
        try:
            self.strBuildFolder = os.path.join(STR_WS_FOLDER, "demo", "build")
            sys.path.append(self.strBuildFolder)
            clsConfig = importlib.import_module("demos.config")
            self.objConfig = clsConfig.clsGenerate()
        except Exception as e:
            log.debug("exception clsPrepromote fnSetup")
            raise e

    def fnGetDemos(self):
        try:
            os.chdir(self.strBuildFolder)
            self.objConfig.scanDemos()
            self.objConfig.scanToolchains()
            self.listBorads = os.listdir(STR_BOARDS_FOLDERS)
            listDemos = self.fnScanDemos()
            listToolchains = self.fnScanToolchains()
            self.fnSetDemos(listDemos, listToolchains)
        except Exception as e:
            log.debug("exception clsPrepromote fnGetDemos")
            raise e

    def fnScanDemos(self):
        try:
            listDemos = self.objConfig.listDemos
            log.info("Found %s demos:" % len(listDemos))
            for strDemo in sorted(listDemos):
                log.info("%s: %s" % (listDemos.index(strDemo), strDemo))
            log.info("")
            log.info("=" * 35)
            log.info("")
        except Exception as e:
            log.debug("exception clsPrepromote fnScanDemos")
            raise e
        return listDemos

    def fnScanToolchains(self):
        try:
            listToolchains = self.objConfig.listToolchain
            log.info("Found %s Toolchains:" % len(listToolchains))
            for strToolchain in sorted(listToolchains):
                log.info("%s: %s" % (listToolchains.index(strToolchain), strToolchain))
            log.info("")
            log.info("=" * 35)
            log.info("")
        except Exception as e:
            log.debug("exception clsPrepromote fnScanToolchains")
            raise e
        return listToolchains

    def fnSetDemos(self, listDemos, listToolchains):
        try:
            for strDemo in listDemos:
                strModule = STR_MODULE_PATTERN % strDemo
                objModule = importlib.import_module(strModule)
                intToolChain = self.objConfig.fnAutoPickToolchain(listDemos.index(strDemo))
                self.dictDemos.setdefault(strDemo, objModule.demo())
                if intToolChain:
                    setattr(self.dictDemos[strDemo], 'listToolchains', [listToolchains[intToolChain]])
                else:
                    setattr(self.dictDemos[strDemo], 'listToolchains', listToolchains)
        except Exception as e:
            log.debug("exception clsPrepromote fnSetDemos")
            raise e

    def fnGetListOfTargetsToRun(self):
        if self.fileExpectedResultsCsv != None:
            return next(csv.reader(self.fileExpectedResultsCsv))
        return []

    # execute all demos
    def fnRunAll(self):
        try:
            # loop all demos
            for strDemo in sorted(self.dictDemos):
                self.dictResults.setdefault(strDemo, {})
                objDemo = self.dictDemos[strDemo]
                log.info("Executing demo: %s" % strDemo)
                log.info("Found %s supported targets: [ %s ]" % (
                len(objDemo.listDemoSpecificTargets), ", ".join(objDemo.listDemoSpecificTargets)))
                # loop all toolchains
                for strToolChain in objDemo.listToolchains:
                    log.info("Using \"%s\" toolchain" % strToolChain)
                    self.objConfig.fnSaveConfigToFile(strDemo, strToolChain)
                    # loop all targets
                    for strTarget in objDemo.listDemoSpecificTargets:
                        listBoards = self.fnGetDemoBoards(strTarget)
                        for strBoard in listBoards:
                            strConfig = strBoard
                            boolBuildFlag = False
                            dictConfig = objData.configuration[strBoard]
                            # loop all interfaces for each target
                            for interface in dictConfig["interface"]:
                                strName = "%s-%s" % (strTarget, interface)
                                if self.strExpectedHeaderLine and strName not in self.strExpectedHeaderLine:
                                    log.info("skipping %s" % strName)
                                    continue
                                # build only once
                                if boolBuildFlag == False:
                                    boolBuildFlag = True
                                    # build failed
                                    if self.fnBuild(strDemo, strTarget) != 0:
                                        log.error("-----------------------------------")
                                        log.error("Failed to build %s for %s" % (strDemo, strTarget))
                                        log.error("-----------------------------------")
                                        for intf in dictConfig["interface"]:
                                            self.fnLogResults(strToolChain, intf, strTarget, strDemo, STR_FAILED)
                                        #no need to continue to other interfaces since build fails
                                        break
                                # prepare all run parameters
                                interfaceDescriptorDict = dictConfig["interface_descriptor"]
                                interfaceDictConfig = interfaceDescriptorDict[interface]
                                interfaceDictConfig["name"] = strConfig
                                interfaceDictConfig["wd"] = os.path.join(STR_BOARDS_FOLDERS, strBoard)
                                interfaceDictConfig["elf"] = os.path.join(STR_OUTPUT_FOLDER, strTarget + STR_BIN_EXT)
                                interfaceDictConfig["board_wd"] = os.path.join(STR_BOARDS_FOLDERS, strBoard)
                                interfaceDictConfig["board"] = strBoard
                                interfaceDictConfig["target"] = strTarget
                                interfaceDictConfig["toolchain"] = strToolChain
                                interfaceDictConfig["interface_name"] = interface
                                interfaceDictConfig["gdb"] = objData.gdb
                                log.debug(interfaceDictConfig)
                                log.debug("fnRunAll start")
                                # create an instance of the platform we are about to run
                                self.platformObj = clsPlatform(interfaceDictConfig)
                                intRetryCount = 0
                                # we loop twice just in case the first run failed
                                while intRetryCount < 2:
                                    # run the demo
                                    strLogFileResult = self.platformObj.fnRun()
                                    os.system("echo \"\n%s\" >> %s" %(STR_END_OF_LOG, strLogFileResult))
                                    strResult = self.fnParseResults(strToolChain, interface, strTarget, strDemo, strLogFileResult)
                                    log.info(strResult)
                                    # if the demo failed we go for a second run
                                    if strResult == STR_PASSED:
                                        # will cause the loop to break
                                        intRetryCount = 2
                                    else:
                                        intRetryCount = intRetryCount + 1
                                        if intRetryCount < 2: 
                                            log.info("Rerun: %s %s" % (strBoard, strDemo))
                                # log the demo run results
                                self.fnLogResults(strToolChain, interface, strTarget, strDemo, strResult)
                                self.platformObj = None
                                log.debug("fnRunAll end")
        except Exception as e:
            log.debug("exception clsPrepromote fnRunAll")
            raise e

    # parse the demo results
    # this function will look in the log file and search for the text 'start' and 'stop'/'error'
    # and will return 'passed'/'failed' accordingly
    def fnParseResults(self, strToolChain, interface, strTargetName, strDemoName, strResultFile):
        try:
            boolStart = False
            boolEnd = False
            strResult = STR_PASSED
            f = open(strResultFile, "r")
            while True:
                line = f.readline()
                # demo started?
                if STR_TEST_STARTED in line:
                    boolStart = True
                # demo ended?
                elif STR_TEST_ENDED in line:
                    boolEnd = True
                    break
                # demo had an error or aborted?
                elif STR_TEST_ERROR in line or STR_END_OF_LOG in line:
                    break
            f.close()
            # if no indication for start or end - this means the demo had failed 
            if boolStart == False or boolEnd == False:
                strResult = STR_FAILED
        except Exception as e:
            log.debug("exception clsPrepromote fnParseResults")
            raise e
        return strResult

    # log the results
    def fnLogResults(self, strToolChain, interface, strTargetName, strDemoName, strResult):
        try:
            if strResult == STR_FAILED:
                # global mark we failed
                self.boolDemosFail = True
                # add to the list of failed demos - will be displayed at the end of the prepromote run
                self.listDemoFailedResults.append([strToolChain, interface, strTargetName, strDemoName, strResult])
            # log the results
            self.listDemoResults.append([strToolChain, interface, strTargetName, strDemoName, strResult])
        except Exception as e:
            log.debug("exception clsPrepromote fnLogResults")
            raise e

    # compile and link a specific demo
    def fnBuild(self, strDemo, strTarget):
        try:
            print STR_BUILD_CMD % strTarget
            # clear before build
            intRet = os.system(STR_CLEAN_CMD % strTarget)
            self.dictResults[strDemo].setdefault("Clean", intRet)
            if not intRet:
                # build
                intRet = os.system(STR_BUILD_CMD % strTarget)
                # was interrupted by the user
                if intRet == 2:
                    raise Exception("KeyboardInterrupt user aborted")
                self.dictResults[strDemo].setdefault("Build", intRet)
            else:
                self.dictResults[strDemo].setdefault("Build", -1)
        except Exception as e:
            log.debug("exception clsPrepromote fnBuild")
            raise e
        return intRet

    # get a list of the boards to run with a specific demo
    def fnGetDemoBoards(self, strTarget):
        try:
            listBoards = []
            for strFolder in self.listBoardsFolders:
                if strTarget in strFolder:
                    listBoards.append(strFolder)
        except Exception as e:
            log.debug("exception clsPrepromote fnGetDemoBoards")
            raise e
        return listBoards

    # output the results
    def fnPrintResults(self):
        try:
            platformsList = []
            # build list of platforms
            platformsList.append("demo name")
            for demoRes in self.listDemoResults:
                if demoRes[2]+'-'+demoRes[1] not in platformsList:
                    platformsList.append(demoRes[2]+'-'+demoRes[1])
            # build platforms dict
            platformDict = {}
            platformDict.setdefault("demo name", 0)
            for platform in platformsList:
                platformDict.setdefault(platform, platformsList.index(platform))
            listDemos = []
            # demos list
            for strDemo in sorted(self.dictDemos):
                listDemos.append(strDemo)
            demosDict = {}
            # create demos dict
            for strDemo in sorted(listDemos):
                demosDict.setdefault(strDemo, listDemos.index(strDemo))
            intNumOfCol = len(demosDict)
            csvTable = [["n/a" for i in range(1+len(platformDict))] for j in range(len(demosDict)*2)]
            csvFileHandle = open(os.path.join(STR_CI_FOLDER, "checklist.csv"), "w+")
            csvFileHandle.write(','.join(platformsList))
            csvFileHandle.write("\n")
            # log all the results to a CSV file
            for listDemoResult in self.listDemoResults:
                if listDemoResult[INT_TOOLCHAIN_INDEX] =="gcc":
                    row = 0
                else: 
                    row = intNumOfCol
                row = row + demosDict[listDemoResult[INT_DEMO_NAME_INDEX]]
                col = platformDict[listDemoResult[INT_TARGET_NAME_INDEX]+'-'+listDemoResult[INT_PLATFORM_NAME_INDEX]]
                csvTable[row][col] = listDemoResult[INT_RESULT_INDEX]
                csvTable[row][len(platformsList)] = listDemoResult[INT_TOOLCHAIN_INDEX]
            for strDemo in sorted(listDemos):
                csvTable[demosDict[strDemo]][0] = strDemo
                csvTable[demosDict[strDemo]+intNumOfCol][0] = strDemo
            for row in csvTable:
                csvFileHandle.write(','.join(row))
                csvFileHandle.write("\n")
            
            # if end user provided a CSV file, we need to compare our results
            # with the given file - maybe some test did fail but it is expected
            if self.fileExpectedResultsCsv:
                csvFileHandle.seek(0, 0)
                self.boolDemosFail = not self.fnCompareCsvFiles(self.fileExpectedResultsCsv, csvFileHandle)
                self.fileExpectedResultsCsv.close()

            csvFileHandle.close()

            if self.boolDemosFail == True:
                print("Failed demos:")
                for failedDemo in self.listDemoFailedResults:
                    print(failedDemo)
                print("   ______    _ _  ")
                print("  |  ____|  (_) | ")
                print("  | |__ __ _ _| | ")
                print("  |  __/ _` | | | ")
                print("  | | | (_| | | | ")
                print("  |_|  \__,_|_|_| ")
            else:
                print("  _____               ")
                print(" |  __ \              ")
                print(" | |__) |_ _ ___ ___  ")
                print(" |  ___/ _` / __/ __| ")
                print(" | |  | (_| \__ \__ \ ")
                print(" |_|   \__,_|___/___/ ")
            print(" ")
        except Exception as e:
            log.debug("exception clsPrepromote fnPrintResults")
            raise e

    # this function compares our CSV results with the end user provided CSV file
    def fnCompareCsvFiles(self, fileBaseCsv, fileNewCsv):
        try:
            boolComparePass = True
            # skip new csv header
            strNewCsvFileLine = next(csv.reader(fileNewCsv))
            strLineNewCsvFileLines = iter(csv.reader(fileNewCsv))
            strLineBaseCsvFileLines = iter(csv.reader(fileBaseCsv))
            for strBaseCsvFileLine, strNewCsvFileLine in zip(strLineBaseCsvFileLines, strLineNewCsvFileLines):
                # if not match, print the diffs
                if strNewCsvFileLine != strBaseCsvFileLine:
                    boolComparePass = False
                    log.info("          %s" % self.strExpectedHeaderLine)
                    log.info("Expected: %s" % strBaseCsvFileLine)
                    log.info("Recieved: %s\n" % strNewCsvFileLine)
        except Exception as e:
            log.debug("exception clsPrepromote fnCompareCsvFiles")
            raise e
        return boolComparePass

    # allow clean exit in case of exception
    def fnExit(self):
        if self.platformObj != None:
            print("Before self.platformObj.fnTerminateCurrentExecution(True)")
            self.platformObj.fnTerminateCurrentExecution(True)
            print("After self.platformObj.fnTerminateCurrentExecution(True)")
        exit(1)

# parse user arguments
def fnParseArguments():
    try:
        listArgs = []
        parser = argparse.ArgumentParser()
        parser.add_argument('--exp', dest='csv', help='expected results (csv file)', default=None, type=file)
        parser.add_argument('--listdemo', dest='listdemo', help='comma seperated list to execute', default=None, type=str)
        args = parser.parse_args()
        for arg in vars(args):
            listArgs.append(getattr(args, arg))
        if len(listArgs):
            log.info("python arguments:")
            log.info(listArgs)
    except Exception as e:
        raise e
    return listArgs

if __name__ == "__main__":
    try:
        log.info("Prepromote starting .... ")
        # parse end user arguments
        listArgs = fnParseArguments()
        # read prepromote.json file
        objData = clsData(listArgs[INT_DEMO_LIST_ARG_INDEX])
        objPrepromote = clsPrepromote(listArgs[INT_CSV_FILE_ARG_INDEX])
        # run all demos
        objPrepromote.fnRunAll()
        # print results
        objPrepromote.fnPrintResults()
        log.info("Prepromote End .... ")
        # close the CSV provided by the end user (if provided)
        if listArgs[0] != None:
            listArgs[0].close();
    except Exception as e:
        # log we got an exception
        log.error("Prepromote exception:")
        log.error(e)
        log.error(traceback.print_exc())
        # exit due to the exception
        objPrepromote.fnExit()
    # if we had a failure, exit with 1
    if objPrepromote.boolDemosFail == True:
        exit(1)
    # not error - exit
    exit(0)

'''
class clswhisperZZZ(object):
    def __init__(self, dictConfig):
        self.dictConfig = dictConfig
        self.strCMD = "./whisper"
        self.objProcess = object
        self.fnRun()

    def fnRun(self):
        objSerialData = clsReadPts(self.dictConfig["board"])
        objGdb = clsGdb(self.dictConfig)
        objSerialData.fnStart()
        objGdb.fnStart()
        while True:
            if objSerialData.boolTestDone:
                objGdb.fnDistructProcess()
                self.fnDistructProcess()
                break

    def fnStart(self):
        None

    def fnFlush(self):
        None

    def fnTerminateProcess(self):
        whisperPid = int(check_output(["pidof","-s", "whisper"]))
        os.kill(whisperPid, signal.SIGKILL)
'''
