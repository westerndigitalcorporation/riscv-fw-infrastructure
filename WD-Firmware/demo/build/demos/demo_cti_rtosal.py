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
    self.strDemoName   = "cti_rtos"
    self.rtos_core     = ""
    self.toolchain     = ""
    self.toolchainPath = ""
    self.strGrpFile    = os.path.join("..", "cti.csv")
    self.strComrvCacheSize = "16896"
    self.strLinkFilePrefix = ''
    self.strComrvCacheAlinmentSize = '512'
    self.strOverlayStorageSize = "33792"

    self.public_defs = [
        'D_USE_FREERTOS',
        'D_USE_RTOSAL',
        'D_TICK_TIME_MS=10',
        'D_ISR_STACK_SIZE=400',
        'D_COMRV_ENABLE_RTOS_SUPPORT',
        'D_COMRV_ENABLE_ERROR_NOTIFICATIONS',
        'D_COMRV_MIN_GROUP_SIZE_IN_BYTES=512',
        'D_COMRV_MAX_GROUP_SIZE_IN_BYTES=4096',
        'D_COMRV_MAX_CALL_STACK_DEPTH=10',
        'D_COMRV_MAX_OVL_CACHE_SIZE_IN_BYTES='+self.strComrvCacheSize,
        'D_CTI',
        'D_CTI_RTOS',
        'D_COMRV_ENABLE_MULTI_GROUP_SUPPORT',
        'D_COMRV_ENABLE_MIN_NUM_OF_MULTI_GROUP_SUPPORT',
        'D_COMRV_ENABLE_CONTROL_SUPPORT',
        'D_COMRV_ENABLE_CRC_SUPPORT',
        'D_COMRV_ENABLE_OVL_DATA_SUPPORT',
    ]

    self.listSconscripts = [
      'freertos',
      'rtosal',
      'comrv',
      'cti',
      'demo_cti_rtosal',
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
      # size of the overlay storage 
      '-Wl,--defsym=__comrv_overlay_storage_size=' + self.strOverlayStorageSize,
      #'-Wl,--comrv-debug',
    ]

    self.listDemoSpecificTargets = [
      'eh2', 'el2',
    ]
