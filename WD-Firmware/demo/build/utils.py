#/* 
#* SPDX-License-Identifier: Apache-2.0
#* Copyright 2019 Western Digital Corporation or its affiliates.
#* 
#* Licensed under the Apache License, Version 2.0 (the "License");
#* you may not use this file except in compliance with the License.
#* You may obtain a copy of the License at
#* 
#* http:*www.apache.org/licenses/LICENSE-2.0
#* 
#* Unless required by applicable law or agreed to in writing, software
#* distributed under the License is distributed on an "AS IS" BASIS,
#* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#* See the License for the specific language governing permissions and
#* limitations under the License.
#*/

import os
import platform

STR_HEADER_BORDER = '\n _______________________________________\n'

STR_NAMES_CONCATENATE = "%s_%s"
STR_MAP_FILE_EXT = ".map"
STR_ELF_FILE_EXT = ".elf"
STR_DMP_FILE_EXT = ".dmp"

STR_HEADER_TEXT = "Sections size summary"
STR_MAP_SECTION_APPEND = "echo \"%s\" >> %s"
STR_MAP_SIZE_APPEND = "%s %s >> %s"

##### Dependencies check
STR_LIST_PKGS = "dpkg -s %s > tmp.txt 2>&1"
STR_TMP_FILE = "tmp.txt"
STR_PLATFORM = "Linux"
STR_NO_INSTALL = "not installed"
#todo : consider using SCons API: Execute(Delete("the file))
if platform.uname()[0] == STR_PLATFORM:
    STR_REMOVE_FILE = "rm %s"
else:
    STR_REMOVE_FILE = "del %s"
    

STR_TC_LLVM                  = "llvm"
STR_BINUTILS                 = "binutils"
STR_TC_GCC                   = "gcc"

STR_TOOLCHAIN            = "toolchain"
STR_BIN_FOLDER           = "bin"

STR_NEW_LINE = "\n"

STR_OVL_DATA_SEC_NAME = ".ovlgrps"
STR_RESERVED_OVL_SEC_NAME = ".reserved_ovl"
INT_SEC_ADDR_INDEX = 3
INT_SEC_SIZE_INDEX = 5

# this function creates a header string
def fnHeaderCreate(listHeader):
   strHeader = ""
   for strLine in listHeader:
      strHeader += '\n\t' + strLine
   return STR_HEADER_BORDER + strHeader + STR_HEADER_BORDER
   
# add 'size' util output to the map file
def fnProduceSectionsSize(target, source, env):
   f  = open(env['MAP_FILE'], "r")
   # check if there was any change in the .map file
   if not STR_HEADER_TEXT in f.read():
      # .map was updated so add the sections size
      strElfName = str(source[0])
      strSizeUtilName = os.path.join(env['UTILS_BASE_DIR'], "bin", env['SIZE_BIN'])
      strHeader = fnHeaderCreate([STR_HEADER_TEXT])
      fnExecuteCommand(STR_MAP_SECTION_APPEND % (strHeader, env['MAP_FILE']))
      fnExecuteCommand(STR_MAP_SIZE_APPEND % (strSizeUtilName, strElfName, env['MAP_FILE']))
   f.close()
   return None

# create the dump file
def fnProduceDump(target, source, env):
   strDmpName     = env['DMP_FILE']
   strUtilsBaseDir = env[env['STR_TOOL_PREFIX']+'UTILS_BASE_DIR']
   strDumpUtil = env[env['STR_TOOL_PREFIX']+'OBJDUMP_BIN']
   strAttr = env[env['STR_TOOL_PREFIX']+'OBJDUMP_ATTR']
   strDmpUtilName = os.path.join(strUtilsBaseDir, "bin", strDumpUtil)
   strcmd = strDmpUtilName + ' ' + env['ELF_FILE'] + strAttr + ' -DSh > ' + strDmpName
   fnExecuteCommand(strDmpUtilName + ' ' + env['ELF_FILE'] + strAttr + ' -DSh > ' + strDmpName)
   return None

# move overlay section from virtual address to flash address 
def fnCopyOverlaySection(target, source, env):
   strReadElfUtilName = os.path.join(env['UTILS_BASE_DIR'], "bin", env['READELF_BIN'])
   strObjcopyUtilName = os.path.join(env['UTILS_BASE_DIR'], "bin", env['OBJCOPY_BIN'])
   # get elf sections and save them to STR_TMP_FILE
   fnExecuteCommand(strReadElfUtilName + ' -S ' + env['ELF_FILE'] + ' > ' + STR_TMP_FILE)
   # read STR_TMP_FILE
   f = open(STR_TMP_FILE, "r")
   strElfSections = f.read()
   f.close()
   intOvlSectionSize = 0
   listSectionProperties = []
   listLines = strElfSections.split(STR_NEW_LINE)
   for strLine in listLines:
     # remove leading '[ '
     strLine = strLine.replace('[ ', '', 1)
     # search for the reserved overlay section
     if strLine.find(STR_RESERVED_OVL_SEC_NAME) >= 0:
       listSectionProperties.extend(strLine.split())
       # save the section address and size
       strReservedSectionAddress = listSectionProperties[INT_SEC_ADDR_INDEX]
       intReservedSectionSize    = int(listSectionProperties[INT_SEC_SIZE_INDEX], 16)
       listSectionProperties = []
     # search for the overlay data section
     elif strLine.find(STR_OVL_DATA_SEC_NAME) >= 0:
       listSectionProperties.extend(strLine.split())
       # save the section size
       intOvlSectionSize    = int(listSectionProperties[INT_SEC_SIZE_INDEX], 16)
       listSectionProperties = []
   # if we have overlays
   if intOvlSectionSize != 0:
      # verify the overlay groups section size fits the reserved overlay section size
      if intOvlSectionSize <= intReservedSectionSize:
         # dump the ovelay section
         str = "%s %s --dump-section %s=%s" % (strObjcopyUtilName, env['ELF_FILE'], STR_OVL_DATA_SEC_NAME, STR_TMP_FILE)
         fnExecuteCommand(str)
         # modify the reserved section flags - it is missing the content flag
         str = "%s %s --set-section-flags %s=contents,alloc" % (strObjcopyUtilName, env['ELF_FILE'], STR_RESERVED_OVL_SEC_NAME)
         fnExecuteCommand(str)
         # update the reserved section from the dumped section
         str = "%s %s --update-section %s=%s" % (strObjcopyUtilName, env['ELF_FILE'], STR_RESERVED_OVL_SEC_NAME, STR_TMP_FILE)
         fnExecuteCommand(str)
      else:
         print ("Error: can't move .ovlgrpdata")
         print "'%s' is too small [%s] while '%s' size is [%s]" %(STR_RESERVED_OVL_SEC_NAME, hex(intReservedSectionSize), STR_OVL_DATA_SEC_NAME, hex(intOvlSectionSize))
         fnExecuteCommand("rm " + env['ELF_FILE'])
   # delete the temporary file
   fnExecuteCommand(STR_REMOVE_FILE % STR_TMP_FILE, "unable to delete temporary file")
   
   return None

#TODO[OS]: we need to see if this is needed for Windows
# under linux, verify installation dependencies
def fnCheckInstalledDependencis(listDependencis):
  if platform.uname()[0] == STR_PLATFORM:
    for strDependency in listDependencis:
      fnExecuteCommand(STR_LIST_PKGS % strDependency, "dpkg failed executing")
      if STR_NO_INSTALL in open(STR_TMP_FILE).read():
        print("Error: please install missing library - " + strDependency)
        fnExecuteCommand(STR_REMOVE_FILE % STR_TMP_FILE)
        exit(1)

    fnExecuteCommand(STR_REMOVE_FILE % STR_TMP_FILE, "Remove temporary file failed")
  else: 
    # currently only linux is supported 
    print("unsupported environment, please switch to a linux based machine")
    exit(1)

def fnSetOutputFileNames(prefix = ""):
    # format the artifacts files name for the current demo
    #return a list
    map_file = prefix + STR_MAP_FILE_EXT
    elf_file = prefix + STR_ELF_FILE_EXT
    dmp_file = prefix + STR_DMP_FILE_EXT
    return map_file, elf_file, dmp_file

# set toolchain path
def fnSetToolchainPath(strTCName, env):
    if strTCName == STR_TC_LLVM:
       env['RISCV_LLVM_TC_PATH'] = os.path.join(os.getcwd(), STR_TOOLCHAIN, STR_TC_LLVM)
       # check if the TC folder exist
       if not os.path.isdir(env['RISCV_LLVM_TC_PATH']):
         print ("Error: No LLVM Toolchain found at: %s" % env['RISCV_LLVM_TC_PATH'])
         exit(1)
       else:
         print "Setting LLVM Toolchain to => %s" % env['RISCV_LLVM_TC_PATH']

       # check if the Binutils folder exist
       env['RISCV_BINUTILS_TC_PATH'] = os.path.join(env['RISCV_LLVM_TC_PATH'], STR_TC_GCC)
       env['UTILS_BASE_DIR']         = env['RISCV_BINUTILS_TC_PATH'] 
       env['LLVM_UTILS_BASE_DIR'] = env['RISCV_LLVM_TC_PATH']
       if not env['RISCV_BINUTILS_TC_PATH']:
         print ("Error: No Binutils found at: %s" % env['RISCV_BINUTILS_TC_PATH'])
         exit(1)
       else:
         print "Setting Binutils Toolchain to => %s" % env['RISCV_BINUTILS_TC_PATH']

    elif strTCName == STR_TC_GCC:
       env['RISCV_GCC_TC_PATH'] = os.path.join(os.getcwd(), STR_TOOLCHAIN, STR_TC_GCC)
       env['UTILS_BASE_DIR']    = env['RISCV_GCC_TC_PATH'] 
       # check if the TC folder exist
       if not os.path.isdir(env['RISCV_GCC_TC_PATH']):
         print ("Error: No GCC Toolchain found at: %s" % env['RISCV_GCC_TC_PATH'])
         exit(1)
       else:
         print "Setting GCC Toolchain to => %s" % env['RISCV_GCC_TC_PATH']

    else:
      print ("Error: No toolchain present for : %s" %strTCName)
      exit(1)

    # setting up a bin folder for the debugger
    strBinFolder = os.path.join(env['UTILS_BASE_DIR'], STR_BIN_FOLDER)
    linkGDBFolder(strBinFolder)

# add a bin folder for the GDB to run from it
def linkGDBFolder(strBinFolder):
    strGDBFolder = os.path.join(os.getcwd(), STR_TOOLCHAIN, STR_BIN_FOLDER)

    # delete previously linked binaries as toolchain may have been changed
    if os.path.isdir(strGDBFolder):
        fnExecuteCommand("rm -r %s" % strGDBFolder, "Could not remove folder %s\nRemove it manually then try again" % strGDBFolder)


    os.mkdir(strGDBFolder)

    # link all bin files while renaming riscv32 to riscv64
    for strFile in os.listdir(strBinFolder):
        strLinkFile = strFile
        if "riscv32" in strFile:
            strLinkFile = strFile.replace("riscv32", "riscv64")

        strCmd = "ln -s %s %s" % (os.path.join(strBinFolder, strFile), os.path.join(strGDBFolder, strLinkFile))
        fnExecuteCommand(strCmd, "Creating symbolic link folder at: %s" % strGDBFolder)


# get toolchain specific comiler/linker flags
def fnGetToolchainSpecificFlags(strTCName, env):
    if strTCName == STR_TC_LLVM:
      listSpecificLinkerOptions = ['']
      listSpecificCFlagsOptions = ['--gcc-toolchain='+ env['RISCV_BINUTILS_TC_PATH'],
                                   '--sysroot=' + os.path.join(env['RISCV_BINUTILS_TC_PATH'],'riscv32-unknown-elf')]
    elif strTCName == STR_TC_GCC:
      listSpecificLinkerOptions = ['']
      listSpecificCFlagsOptions = ['']
    else:
      print ("Error: No toolchain present for:%s" %strTCName)
      exit(1)

    return listSpecificCFlagsOptions, listSpecificLinkerOptions

def fnCreateFolder(strPath):
  if not os.path.exists(strPath):
    os.makedirs(strPath)

def fnExecuteCommand(strCmd, strErr = ""):
  retSystCall = os.system(strCmd)
  if retSystCall != 0:
    print ("Error: %s" % strCmd)
    if strErr: 
      print ("Error: %s" % strErr)
    exit(1)

def fnGetDefine(strDefine, listDefines):
  for strdef in listDefines:
    if strDefine in strdef:
      return strdef.split("=")[0]
  return None
