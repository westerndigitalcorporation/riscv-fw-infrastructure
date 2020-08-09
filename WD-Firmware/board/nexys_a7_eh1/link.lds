/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright 2019 Western Digital Corporation or its affiliates.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/ 

/*
 Linker script
*/

OUTPUT_ARCH( "riscv" )

ENTRY( _start )

MEMORY
{
  ram  (wxa!ri) : ORIGIN = 0x00000000, LENGTH = 64M
  ram2 (wxa!ri) : ORIGIN = 0x04000000, LENGTH = 64M
  dccm (wxa!ri) : ORIGIN = 0xf0040000, LENGTH = 64K
  ovl           : ORIGIN = 0xE0000000, LENGTH = 8k
}

PHDRS
{
  rom_load PT_LOAD;
  ram_init PT_LOAD;
  ram_load PT_LOAD;
  ovl PT_NULL;
}


/*----------------------------------------------------------------------*/
/* Sections                            */
/*----------------------------------------------------------------------*/

SECTIONS
{
  __stack_size = DEFINED(__stack_size) ? __stack_size : 4K;
  /* __comrv_cache_alignment_size defines the alinment size of the start cache area */
  __comrv_align_size = __comrv_cache_size != 0 ? __comrv_cache_alignment_size : 0;

  .text.init :
  {
    *(.text.init)
    . = ALIGN(8);
  } > ram : ram_load

  .text :
  {
    *(.text.unlikely .text.unlikely.*)
    *(.text.startup .text.startup.*)
    *(.text .text.*)
    *(COMRV_TEXT_SEC)
    *(COMRV_TEXT_ASM)
    *(.gnu.linkonce.t.*)
    . = ALIGN(4);
  } >ram : ram_load


  .rodata :
  {
    *(.rdata)
    *(.rodata .rodata.*)
    *(.gnu.linkonce.r.*)
    KEEP(*(COMRV_RODATA_SEC))
    . = ALIGN(4);
  } > ram : ram_load

  /* this is a placeholder for comrv overlay groups.
     after linking, we'll use objcopy utility and change the
     address of .ovlgrpdata to this address */ 
  .reserved_ovl : ALIGN(4)
  {
    _OVERLAY_STORAGE_START_ADDRESS_ = .;
    . = 0x2000;
  } > ram : ram_load

  
  .lalign : 
  {
    . = ALIGN(4);
    PROVIDE( _data_lma = . );
  } > ram : ram_load
  
  .dalign :
  {
    . = ALIGN(4);
    PROVIDE( _data = . );
  } > ram : ram_load
  
  
  .data :
  {
    *(.data .data.*)
    *(.gnu.linkonce.d.*)
    . += 10; /* fix for linker false error message */
    . = ALIGN(8); 
  } > ram : ram_load

  /* 4 bytes reserved for software interrupt memory control
     used by whisper only! to trigger sw interrupt 
     this must be first in ram2 - don't change it's location */
  SW_INT : ALIGN(4)
  {
    _sw_int_mem_ctrl = .;
    . = 4;
  }  > ram2 : ram_load

  COMRV_DATA_SEC : ALIGN(16)
  {
  
  }  > ram2 : ram_load

  PSP_DATA_SEC : ALIGN(16)
  {
  
  }  > ram2 : ram_load

  .sdata :
  {
    . = ALIGN(8);
    __global_pointer$ = . + 0x800;
    *(.sdata .sdata.*)
    *(.gnu.linkonce.s.*)
    . = ALIGN(8);
    *(.srodata .srodata.*)
   . = ALIGN(8);
  } > ram : ram_load


  
  . = ALIGN(4);
  PROVIDE( _edata = . );
  PROVIDE( edata = . );

  PROVIDE( _fbss = . );
  PROVIDE( __bss_start = . );
  
  .bss :
  {
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
    *(.bss)
    . = ALIGN(8);
  } >ram : ram_load

  _end = .;

  .stack :
  {
    _heap_end = .;
    . = . + __stack_size;
    _sp = .;
  } > ram : ram_load
  
  
  /* this area is used for loading and executing the comrv 
     overlay groups */
  .overlay_sec ALIGN(__comrv_align_size) :
  {
    __OVERLAY_CACHE_START__ = . ;
    . = __comrv_cache_size; 
    __OVERLAY_CACHE_END__ = . ;
  } > ram : ram_load

  /* this is the location of all comrv overlay groups.
     after linking, we'll use objcopy utility and copy
     it to the .reserved_ovl section. we do this
     to make sure there is no address dependency after linking */ 
  .ovlgrps (OVERLAY) : 
  {
    OVERLAY_START_OF_OVERLAYS = .;
    *(.ovlinput.*)
    OVERLAY_END_OF_OVERLAYS = .;
  } >ovl AT>ovl :ovl
}
