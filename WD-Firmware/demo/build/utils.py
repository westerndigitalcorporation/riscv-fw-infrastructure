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
STR_REMOVE_FILE = "rm %s"

STR_TC_LLVM                  = "llvm"
STR_TC_GCC                   = "gcc"
STR_RV_GCC_ENV_VAR_NAME      = 'RISCV_GCC_TC_ROOT'
STR_RV_LLVM_ENV_VAR_NAME     = 'RISCV_LLVM_TC_ROOT'
STR_RV_BINUTILS_ENV_VAR_NAME = 'RISCV_BINUTILS_ROOT'

STR_NEW_LINE = "\n"

STR_OVL_DATA_SEC_NAME = ".ovlgrpdata"
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
      os.system(STR_MAP_SECTION_APPEND % (strHeader, env['MAP_FILE']))
      os.system(STR_MAP_SIZE_APPEND % (strSizeUtilName, strElfName, env['MAP_FILE']))
   f.close()
   return None

# create the dump file
def fnProduceDump(target, source, env):
   strDmpName     = env['DMP_FILE']
   strDmpUtilName = os.path.join(env['UTILS_BASE_DIR'], "bin", env['OBJDUMP_BIN'])
   os.system(strDmpUtilName + ' ' + env['ELF_FILE'] + ' -DSh > ' + strDmpName)
   return None

# move overlay section from virtual address to flash address 
def fnMoveOverlaySection(target, source, env):
   strReadElfUtilName = os.path.join(env['UTILS_BASE_DIR'], "bin", env['READELF_BIN'])
   strObjcopyUtilName = os.path.join(env['UTILS_BASE_DIR'], "bin", env['OBJCOPY_BIN'])
   # get elf sections and save them to STR_TMP_FILE
   os.system(strReadElfUtilName + ' -S ' + env['ELF_FILE'] + ' > ' + STR_TMP_FILE)
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
   # delete the temporary file
   os.system(STR_REMOVE_FILE % STR_TMP_FILE)
   # if we have overlays
   if intOvlSectionSize != 0:
      str =  "%s --change-section-address %s=0x%s %s %s" % (strObjcopyUtilName, STR_OVL_DATA_SEC_NAME, strReservedSectionAddress, env['ELF_FILE'], env['ELF_FILE'])
      # verify the overlay groups section size fits the reserved overlay section size
      if intOvlSectionSize <= intReservedSectionSize:
         # change the address of .ovlgrpdata section to be the address of the reserved section
         os.system(str)
      else:
         print ("Error: can't move .ovlgrpdata")
         print "'%s' is too small [%s] while '%s' size is [%s]" %(STR_RESERVED_OVL_SEC_NAME, hex(intReservedSectionSize), STR_OVL_DATA_SEC_NAME, hex(intOvlSectionSize))
         os.system("rm " + env['ELF_FILE'])
   return None
# under linux, verify installation dependencies
def fnCheckInstalledDependencis(listDependencis):
  if platform.uname()[0] == STR_PLATFORM:
    for strDependency in listDependencis:
      os.system(STR_LIST_PKGS % strDependency)
      if STR_NO_INSTALL in open(STR_TMP_FILE).read():
        print("Error: please install missing library - " + strDependency)
        os.system(STR_REMOVE_FILE % STR_TMP_FILE)
        exit(1)
    os.system(STR_REMOVE_FILE % STR_TMP_FILE)
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
       env['RISCV_LLVM_TC_PATH'] = os.getenv(STR_RV_LLVM_ENV_VAR_NAME)
       # check if the TC environment variable is set or empty
       if not env['RISCV_LLVM_TC_PATH']:
         print ("Error: Set environment variable '" + STR_RV_LLVM_ENV_VAR_NAME + "' to point to the RISCV llvm root")
         exit(1)
       # check if the binutils environment variable is set or empty
       env['RISCV_BINUTILS_TC_PATH'] = os.getenv(STR_RV_BINUTILS_ENV_VAR_NAME)
       env['UTILS_BASE_DIR']         = env['RISCV_BINUTILS_TC_PATH'] 
       if not env['RISCV_BINUTILS_TC_PATH']:
         print ("Error: Set environment variable '" + STR_RV_BINUTILS_ENV_VAR_NAME + "' to point to the RISCV bunutils root")
         exit(1)
    elif strTCName == STR_TC_GCC:
       env['RISCV_GCC_TC_PATH'] = os.getenv(STR_RV_GCC_ENV_VAR_NAME)
       env['UTILS_BASE_DIR']    = env['RISCV_GCC_TC_PATH'] 
       # check if the TC environment variable is set or empty
       if not env['RISCV_GCC_TC_PATH']:
         print ("Error: Set environment variable '" + STR_RV_GCC_ENV_VAR_NAME + "' to point to the RISCV gcc root")
         exit(1)
    else:
      print ("Error: No toolchain present")
      exit(1)

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
      print ("Error: No toolchain present")
      exit(1)

    return listSpecificCFlagsOptions, listSpecificLinkerOptions

def fnCreateFolder(strPath):
  if not os.path.exists(strPath):
    os.makedirs(strPath)
