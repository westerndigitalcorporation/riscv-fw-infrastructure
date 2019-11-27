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

STR_SIZE_UTIL = "riscv64-unknown-elf-size"
STR_HEADER_TEXT = "Sections size summary"
STR_MAP_SECTION_APPEND = "echo \"%s\" >> %s"
STR_MAP_SIZE_APPEND = "%s %s >> %s"

##### Dependencies check
STR_LIST_PKGS = "dpkg -s %s > tmp.txt 2>&1"
STR_TMP_FILE = "tmp.txt"
STR_PLATFORM = "Linux"
STR_NO_INSTALL = "not installed"
STR_REMOVE_FILE = "rm %s"

# this function creates a header string
def fnHeaderCreate(listHeader):
   strHeader = ""
   for strLine in listHeader:
      strHeader += '\n\t' + strLine
   return STR_HEADER_BORDER + strHeader + STR_HEADER_BORDER
   
 # add 'size' util output to the map file
def fnProduceSectionsSize(target, source, env):
   # check if there was any change in the .map file
   if not STR_HEADER_TEXT in open(env['MAP_FILE']).read():
      # .map was updated so add the sections size
      strElfName = str(source[0])
      strSizeUtilName = os.path.join(env['TOOLS_BASE_DIR'], STR_SIZE_UTIL)
      strHeader = fnHeaderCreate([STR_HEADER_TEXT])
      os.system(STR_MAP_SECTION_APPEND % (strHeader, env['MAP_FILE']))
      os.system(STR_MAP_SIZE_APPEND % (strSizeUtilName, strElfName, env['MAP_FILE']))
   return None

# under linux, verify installation dependencies
def fnCheckInstalledDependencis(listDependencis):
  if platform.uname()[0] == STR_PLATFORM:
    for strDependency in listDependencis:
      os.system(STR_LIST_PKGS % strDependency)
      if STR_NO_INSTALL in open(STR_TMP_FILE).read():
        print("please install missing library - " + strDependency)
        os.system(STR_REMOVE_FILE % STR_TMP_FILE)
        exit(1)
    os.system(STR_REMOVE_FILE % STR_TMP_FILE)
  else: 
    # currently only linux is supported 
    print("unsupported environment, please switch to a linux based machine")
    exit(1)   
    
def set_outputfiles_names(name, prefix = "", suffix = ""):
    # format the artifacts files name for the current demo
    if prefix:
      name = STR_NAMES_CONCATENATE % (prefix, name)
    if suffix:
      name = STR_NAMES_CONCATENATE % (name, suffix)
    map_file = name + STR_MAP_FILE_EXT
    elf_file = name + STR_ELF_FILE_EXT
    return map_file, elf_file
    
def fnCreateFolder(strPath):
  if not os.path.exists(strPath):
    os.makedirs(strPath)
  