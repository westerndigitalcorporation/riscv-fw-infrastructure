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

class demo(object):
  def __init__(self):
    self.strDemoName   = "comrv_baremetal"
    self.rtos_core     = ""
    self.toolchain     = ""
    self.toolchainPath = ""
    self.strGrpFile    = os.path.join("..", "comrv-baremetal.csv")
    self.strComrvCacheSize = "1536"
    self.strLinkFilePrefix = ''
    self.strComrvCacheAlinmentSize = '1024'

    self.public_defs = [
        'D_BARE_METAL',
        'D_TICK_TIME_MS=4',
        'D_ISR_STACK_SIZE=400',
        'D_COMRV_ENABLE_ERROR_NOTIFICATIONS',
        'D_COMRV_MIN_GROUP_SIZE_IN_BYTES=512',
        'D_COMRV_MAX_GROUP_SIZE_IN_BYTES=4096',
        'D_COMRV_MAX_CALL_STACK_DEPTH=10',
        'D_COMRV_MAX_OVL_CACHE_SIZE_IN_BYTES='+self.strComrvCacheSize,
    ]

    self.listSconscripts = [
      'comrv_baremetal',
      'demo_comrv_baremetal',
    ]

    self.listDemoSpecificCFlags = [
      # -fcomrv is used to enable llvm support for overlay functions/data
      '-fcomrv',
    ]

    self.listDemoSpecificLinkerFlags = [
      # provide user defined grouping file (file name is in self.strGrpFile))
      '-Wl,--grouping-file=' + self.strGrpFile,
      # __comrv_cache_size defines in the the size of ram size to reserve for overlay data and overlay functions execution 
      '-Wl,--defsym=__comrv_cache_size=' + self.strComrvCacheSize,
      # __comrv_cache_alignment_size defines the alinment size of the cache area 
      '-Wl,--defsym=__comrv_cache_alignment_size=' + self.strComrvCacheAlinmentSize,
    ]
