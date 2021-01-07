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
strTty = ""


class clsData(object):

    def __init__(self):
        self.dictResults = {}
        dictJSONConfig = self.fnGetJsonFile()
        for key, val in dictJSONConfig.iteritems():
            setattr(self, key, val)

    def fnGetJsonFile(self):
        f = open("prepromote.json", 'r')
        dictData = self.fnByteify(json.load(f))
        f.close()
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

class clsListener(object):

    #def __init__(self, strBoard, strLog, boolStoreResults = True):
    def __init__(self, strLog, boolStoreResults = True):
        #self.strBoard = strBoard
        self.strConnectedListenerIndex = ""
        self.boolListenerDone = False
        self.strLog = strLog
        self.listListeners = []
        self.boolStoreResults = boolStoreResults
        self.NumOfListeners = 0

    def fnStart(self, strListenerIndex, listenerHandle):
        t = threading.Thread(target=self.fnStartListening, args=(strListenerIndex, listenerHandle))
        t.start()

    def fnStartListening(self, strListenerIndex, listenerHandle):
        listData = []
        data = None
        self.NumOfListeners += 1
        log.debug("clsListener fnStartListening enter %s" % strListenerIndex)
        while self.boolListenerDone == False:
             try:
                 data = listenerHandle.readline().strip()
             except Exception as e:
                 log.debug("%s exception %s" % (strListenerIndex, str(e)))
                 pass
             # in some cases we may get \x00 which will cause stuck
             if not data or data.find("\x00") != -1:
                 continue
             log.debug("%s read: %s" % (strListenerIndex, data))
             listData.append(data)
             # capture start of demo
             if STR_TEST_STARTED in data:
                 self.strConnectedListenerIndex = strListenerIndex
                 log.info("%s demo start" % strListenerIndex)
             # capture end of demo only by the selected listener
             elif (self.strConnectedListenerIndex == strListenerIndex) and (STR_TEST_ENDED in data or STR_TEST_ERROR in data):
                 self.boolListenerDone = True
                 #log.debug(listData)
                 log.info("%s demo end" % strListenerIndex)
             # we still have other USB capturing 'end' from old running test on h51 which is still running
             elif self.strConnectedListenerIndex != "" and self.strConnectedListenerIndex != strListenerIndex and (STR_TEST_ENDED in data or STR_TEST_ERROR in data):
                 listData = []
                 break
             elif STR_LISTENER_ABORTED in data:
                 self.boolListenerDone = True
                 log.info("%s demo aborted" % strListenerIndex)
        if self.boolStoreResults and len(listData) > 0:
            log.debug("clsListener fnStartListening save to file")
            f = open(self.strLog, "a")
            f.writelines("\n".join(listData))
            f.close()
        self.NumOfListeners -= 1
        log.debug("clsListener fnStartListening exit %s" % strListenerIndex)

    def fnWaitForCompletion(self, boolForceAbort = False):
        while self.NumOfListeners != 0:
            time.sleep(1)


class clsFileListener(clsListener):

    def __init__(self, strLog):
        super(clsFileListener, self).__init__(strLog, False)
        self.listListeners.append(strLog)
        self.fileHandler = None
        open(strLog, 'w').close()

    def fnStart(self, strListenerIndex = None, listenerHandle = None):
        self.fileHandler = open(self.listListeners[0], "r")
        super(clsFileListener, self).fnStart("file listener", self.fileHandler)

    def fnStartListening(self, strListenerIndex, listenerHandle):
        super(clsFileListener,  self).fnStartListening(strListenerIndex, listenerHandle)
        self.fileHandler.close()

    def fnWaitForCompletion(self, boolForceAbort):
        if boolForceAbort and self.boolListenerDone == False:
            log.debug("clsFileListener fnWaitForCompletion before")
            fileHandler = open(self.listListeners[0], "a")
            fileHandler.write(STR_LISTENER_ABORTED)
            fileHandler.close()
            log.debug("clsFileListener fnWaitForCompletion after")
        super(clsFileListener,  self).fnWaitForCompletion()


class clsSerialListener(clsListener):

    def __init__(self, strLog):
        super(clsSerialListener, self).__init__(strLog)
        self.listListeners = glob.glob("/dev/ttyUSB*")
        if not self.listListeners:
            log.error("Error finding usb drivers")
            exit(1)

    def fnStart(self, strListenerIndex = None, listenerHandle = None):
        for strUsb in self.listListeners:
            strUSBIndex = strUsb.replace("/dev/tty", "")
            serialHandle = serial.Serial(strUsb, 115200)
            serialHandle.timeout = 1
            super(clsSerialListener, self).fnStart(strUSBIndex, serialHandle)

    def fnWaitForCompletion(self, boolForceAbort):
        if boolForceAbort:
            self.boolListenerDone = True
            log.debug("clsSerialListener fnWaitForCompletion")
        super(clsSerialListener,  self).fnWaitForCompletion()


class clsListenerFactory(object):

    def fnCreateListener(self, strInterfaceName, stLogFileName):
        if strInterfaceName == "openocd":
            return clsSerialListener(stLogFileName)
        elif strInterfaceName == "whisper":
            return clsFileListener(stLogFileName)
        else:
            log.error("Error: invalid interface name: %s" % strInterfaceName)
            exit(1)


class clsGdb(object):

    def __init__(self, dictConfig, sessionNumber):
        self.strLocation = "{WS}/demo/build/toolchain/bin".format(WS=STR_WS_FOLDER)
        self.strCMD = os.path.join(self.strLocation, "riscv64-unknown-elf-gdb")
        self.objProcess = object
        self.dictConfig = dictConfig
        self.strSessionNumber = str(sessionNumber)

    def fnStart(self):
        strLog = os.path.join(STR_CI_FOLDER, "%s_gdb_%s.log" % (self.dictConfig["board"],self.strSessionNumber))
        raw_logfile = open(strLog, "wb")
        strCommand = "%s %s -x %s" % (self.strCMD, self.dictConfig["elf"], self.fnCreateGdbCommandFile())
        self.objProcess = subprocess.Popen(shlex.split(strCommand), stdin=subprocess.PIPE, stdout=raw_logfile,
                                           stderr=raw_logfile, shell=False, cwd=self.dictConfig["board_wd"])

    def fnCreateGdbCommandFile(self):
        global strTty
        listData = []
        listData.append(self.dictConfig["llvm"].format(WS=STR_WS_FOLDER))
        for gdbCmd in self.dictConfig["gdb_cmds_" + self.strSessionNumber]:
            listData.append(gdbCmd.format(WS=STR_WS_FOLDER, TTY=strTty, OUTFILEPTS=STR_PTS_OUTFILE))
        strData = "\n".join(cmd for cmd in listData if cmd)
        strCmdFile = os.path.join(STR_CI_FOLDER, "%s_gdb_cmd_%s" % (self.dictConfig["board"], self.strSessionNumber))
        f = open(strCmdFile, "w")
        f.write(strData)
        f.close()
        return strCmdFile

    def fnTerminateProcess(self):
        log.debug("clsGdb fnTerminateProcess(): enter %s" % self.strSessionNumber)
        self.objProcess.terminate()
        self.objProcess.wait()
        while self.objProcess.poll() == None:
            log.debug("clsGdb fnTerminateProcess(): self.objProcess.poll() == None")
            time.sleep(1)
            self.objProcess.kill()
        log.debug("clsGdb fnTerminateProcess(): exit %s" % self.strSessionNumber)

class clsPlatform(object):

    def __init__(self, dictConfig):
        self.dictConfig = dictConfig
        self.objProcess = object
        strTimestamp = str(time.time())
        log.info(strTimestamp)
        self.strLog = os.path.join(STR_CI_FOLDER, "%s_%s.log" % (strTimestamp, self.dictConfig["board"]))

    def fnRun(self):
        self.fnFlush()
        listenerObj = clsListenerFactory().fnCreateListener(self.dictConfig["interface_name"], self.strLog)
        gdbSessions = int(self.dictConfig["gdb_instances"])
        listenerObj.fnStart()
        self.fnStart()
        objGdb = []
        for i in range(gdbSessions):
            time.sleep(2)
            objGdb.append(clsGdb(self.dictConfig, i+1))
            objGdb[i].fnStart()
        intAccumulatedRuntime = 0
        log.debug("clsPlatform fnRun(): while True: enter")
        while True:
            time.sleep(INT_PLATFORM_EXE_DELAY_IN_SECS)
            intAccumulatedRuntime += INT_PLATFORM_EXE_DELAY_IN_SECS
            if listenerObj.boolListenerDone or intAccumulatedRuntime == INT_DEMO_TO_IN_SECONDS:
                self.fnTerminateProcess()
                for i in range(gdbSessions):
                    objGdb[i].fnTerminateProcess()
                listenerObj.fnWaitForCompletion(intAccumulatedRuntime == INT_DEMO_TO_IN_SECONDS)
                break
        log.debug("clsPlatform fnRun(): while True: exit")
        return self.strLog

    def fnStart(self):
        raw_logfile = open(self.strLog, "wb")
        listIFCMD = shlex.split(
            "%s" % self.dictConfig["execute"].format(WS=STR_WS_FOLDER))
        self.objProcess = subprocess.Popen(listIFCMD, stdin=subprocess.PIPE, stdout=raw_logfile,
                                           stderr=raw_logfile, shell=False, cwd=self.dictConfig["board_wd"])

    def fnFlush(self):
        if self.dictConfig["flush"] != "":
            log.info("Flushing %s" % self.dictConfig["board"])
            listCommand = shlex.split("%s" % (self.dictConfig["flush"].format(WS=STR_WS_FOLDER)))
            proc = subprocess.Popen(listCommand, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                    shell=False, cwd=self.dictConfig["board_wd"])
            data, err = proc.communicate()
            if "->DONE!!!!!!!" not in data + err:
                log.error("error in fulshing device %s" % self.dictConfig["board"])
                log.debug(data)
                log.debug(err)
                exit(1)
            log.info("Flushing completed")

    def fnTerminateProcess(self):
        self.objProcess.terminate()
        self.objProcess.wait()
        while self.objProcess.poll() == None:
            log.debug("After termination openocd Still running")


class clsPrepromote(object):

    def __init__(self):
        self.objConfig = object
        self.strBuildFolder = ""
        self.dictDemos = {}
        self.dictResults = {}
        self.dictBoards = {}
        self.listBoardsFolders = os.listdir(STR_BOARDS_FOLDERS)
        self.fnSetup()
        self.fnGetDemos()
        self.DemoEnds = False
        self.ObjIFProcess = object
        self.ObjGDBProcess = object
        self.listDemoResults = []
        self.boolDemosFail = False
        self.listDemoFailedResults = []

    def fnSetup(self):
        self.strBuildFolder = os.path.join(STR_WS_FOLDER, "demo", "build")
        sys.path.append(self.strBuildFolder)
        clsConfig = importlib.import_module("demos.config")
        self.objConfig = clsConfig.clsGenerate()

    def fnGetDemos(self):
        # origWD = os.getcwd()
        os.chdir(self.strBuildFolder)
        self.objConfig.scanDemos()
        self.objConfig.scanToolchains()
        self.listBorads = os.listdir(STR_BOARDS_FOLDERS)
        listDemos = self.fnScanDemos()
        listToolchains = self.fnScanToolchains()
        self.fnSetDemos(listDemos, listToolchains)

    def fnScanDemos(self):
        listDemos = self.objConfig.listDemos
        log.info("Found %s demos:" % len(listDemos))
        for strDemo in sorted(listDemos):
            log.info("%s: %s" % (listDemos.index(strDemo), strDemo))
        log.info("")
        log.info("=" * 35)
        log.info("")
        return listDemos

    def fnScanToolchains(self):
        listToolchains = self.objConfig.listToolchain
        log.info("Found %s Toolchains:" % len(listToolchains))
        for strToolchain in sorted(listToolchains):
            log.info("%s: %s" % (listToolchains.index(strToolchain), strToolchain))
        log.info("")
        log.info("=" * 35)
        log.info("")
        return listToolchains

    def fnSetDemos(self, listDemos, listToolchains):
        for strDemo in listDemos:
            strModule = STR_MODULE_PATTERN % strDemo
            objModule = importlib.import_module(strModule)
            intToolChain = self.objConfig.fnAutoPickToolchain(listDemos.index(strDemo))
            self.dictDemos.setdefault(strDemo, objModule.demo())
            if intToolChain:
                setattr(self.dictDemos[strDemo], 'listToolchains', [listToolchains[intToolChain]])
            else:
                setattr(self.dictDemos[strDemo], 'listToolchains', listToolchains)

    def fnRunAll(self):
        for strDemo in sorted(self.dictDemos):
            self.dictResults.setdefault(strDemo, {})
            objDemo = self.dictDemos[strDemo]
            log.info("Executing demo: %s" % strDemo)
            log.info("Found %s supported targets: [ %s ]" % (
            len(objDemo.listDemoSpecificTargets), ", ".join(objDemo.listDemoSpecificTargets)))
            for strToolChain in objDemo.listToolchains:
                log.info("Using \"%s\" toolchain" % strToolChain)
                self.objConfig.fnSaveConfigToFile(strDemo, strToolChain)
                for strTarget in objDemo.listDemoSpecificTargets:
                    listBoards = self.fnGetDemoBoards(strTarget)
                    for strBoard in listBoards:
                        strConfig = strBoard
                        self.fnBuild(strDemo, strTarget)
                        dictConfig = objData.configuration[strBoard]
                        for interface in dictConfig["interface"]:
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
                            log.debug(interfaceDictConfig)
                            log.debug("fnRunAll start")
                            platformObj = clsPlatform(interfaceDictConfig)
                            intRetryCount = 0
                            while intRetryCount < 2:
                                strLogFileResult = platformObj.fnRun()
                                os.system("echo \"\n%s\" >> %s" %(STR_END_OF_LOG, strLogFileResult))
                                strResult = self.fnParseResults(strToolChain, interface, strTarget, strDemo, strLogFileResult)
                                log.info(strResult)
                                if strResult == STR_PASSED:
                                    intRetryCount = 2
                                else:
                                    intRetryCount = intRetryCount + 1
                                    if intRetryCount < 2: 
                                        log.info("Rerun: %s %s" % (strBoard, strDemo))
                            self.fnLogResults(strToolChain, interface, strTarget, strDemo, strResult)
                            log.debug("fnRunAll end")

    def fnParseResults(self, strToolChain, interface, strTargetName, strDemoName, strResultFile):
        boolStart = False
        boolEnd = False
        strResult = STR_PASSED
        f = open(strResultFile, "r")
        while True:
            line = f.readline()
            if STR_TEST_STARTED in line:
                boolStart = True
            elif STR_TEST_ENDED in line:
                boolEnd = True
                break
            elif STR_TEST_ERROR in line or STR_END_OF_LOG in line:
                break
        f.close()
        if boolStart == False or boolEnd == False:
            strResult = STR_FAILED
        return strResult

    def fnLogResults(self, strToolChain, interface, strTargetName, strDemoName, strResult):
        if strResult == STR_FAILED:
            self.boolDemosFail = True
            self.listDemoFailedResults.append([strToolChain, interface, strTargetName, strDemoName, strResult])
        self.listDemoResults.append([strToolChain, interface, strTargetName, strDemoName, strResult])

    def fnBuild(self, strDemo, strTarget):
        print STR_BUILD_CMD % strTarget
        intRet = os.system(STR_CLEAN_CMD % strTarget)
        self.dictResults[strDemo].setdefault("Clean", intRet)
        if not intRet:
            intRet = os.system(STR_BUILD_CMD % strTarget)
            self.dictResults[strDemo].setdefault("Build", intRet)
        else:
            self.dictResults[strDemo].setdefault("Build", -1)

    def fnGetDemoBoards(self, strTarget):
        listBoards = []
        for strFolder in self.listBoardsFolders:
            if strTarget in strFolder:
                listBoards.append(strFolder)
        return listBoards

    def fnRun(self, strBoard, dictConfig):
        strClass = "cls%s(%s)" % (dictConfig["interface"], dictConfig)
        objClass = eval(strClass)

    def fnPrintResults(self, fileExpectedResultsCsv):
        platformsList = []
        #build list of platforms
        platformsList.append("demo name")
        for demoRes in self.listDemoResults:
            if demoRes[2]+'-'+demoRes[1] not in platformsList:
                platformsList.append(demoRes[2]+'-'+demoRes[1])
        #build platforms dict
        platformDict = {}
        platformDict.setdefault("demo name", 0)
        for platform in platformsList:
            platformDict.setdefault(platform, platformsList.index(platform))
        listDemos = []
        #demos list
        for strDemo in sorted(self.dictDemos):
            listDemos.append(strDemo)
        demosDict = {}
        #create demos dict
        for strDemo in sorted(listDemos):
            demosDict.setdefault(strDemo, listDemos.index(strDemo))
        intNumOfCol = len(demosDict)
        csvTable = [["n/a" for i in range(1+len(platformDict))] for j in range(len(demosDict)*2)]
        csvFileHandle = open(os.path.join(STR_CI_FOLDER, "checklist.csv"), "w+")
        csvFileHandle.write(','.join(platformsList))
        csvFileHandle.write("\n")
        for listDemoResult in self.listDemoResults:
            if listDemoResult[INT_TOOLCHAIN_INDEX] =="gcc":
                row = 0
            else: 
                row = intNumOfCol
            row = row + demosDict[listDemoResult[INT_DEMO_NAME_INDEX]]
            col = platformDict[listDemoResult[INT_TARGET_NAME_INDEX]+'-'+listDemoResult[INT_PLATFORM_NAME_INDEX]]
            csvTable[row][col] = listDemoResult[INT_RESULT_INDEX]
            csvTable[row][7] = listDemoResult[INT_TOOLCHAIN_INDEX]
        for strDemo in sorted(listDemos):
            csvTable[demosDict[strDemo]][0] = strDemo
            csvTable[demosDict[strDemo]+intNumOfCol][0] = strDemo
        for row in csvTable:
            csvFileHandle.write(','.join(row))
            csvFileHandle.write("\n")

        if fileExpectedResultsCsv:
            csvFileHandle.seek(0, 0)
            self.boolDemosFail = not self.fnCompareCsvFiles(fileExpectedResultsCsv, csvFileHandle)
            fileExpectedResultsCsv.close()

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

    def fnCompareCsvFiles(self, fileBaseCsv, fileNewCsv):
        boolComparePass = True
        boolLogHeaderLine = False
        strHeaderLine = None
        strLineNewCsvFileLines = iter(csv.reader(fileNewCsv))
        strLineBaseCsvFileLines = iter(csv.reader(fileBaseCsv))
        for strBaseCsvFileLine, strNewCsvFileLine in zip(strLineBaseCsvFileLines, strLineNewCsvFileLines):
            if strHeaderLine == None:
                strHeaderLine = strBaseCsvFileLine
            if strNewCsvFileLine != strBaseCsvFileLine:
                if boolLogHeaderLine == False:
                    boolLogHeaderLine == True
                    boolComparePass = False
                    log.info("          %s" % strHeaderLine)
                log.info("Expected: %s" % strBaseCsvFileLine)
                log.info("Recieved: %s\n" % strNewCsvFileLine)
        print("ronen")
        return boolComparePass

def fnParseArguments():
    listArgs = []
    if len(sys.argv) > 1:
        parser = argparse.ArgumentParser()
        parser.add_argument('--exp', dest='csv', help='expected results (csv file)', default=None, type=file)
        args = parser.parse_args()
        listArgs.append(args.csv)
    else:
        listArgs.append(None)
    return listArgs

if __name__ == "__main__":
    log.info("Prepromote starting .... ")
    listArgs = fnParseArguments()
    objData = clsData()
    objPrepromote = clsPrepromote()
    objPrepromote.fnRunAll()
    objPrepromote.fnPrintResults(listArgs[0])
    log.info("Prepromote End .... ")
    if listArgs[0] != None:
        listArgs[0].close();
    if objPrepromote.boolDemosFail == True:
        exit(1)
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
