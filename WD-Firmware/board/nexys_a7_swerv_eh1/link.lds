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
  /* --------  flash (rxai!w) : ORIGIN = 0x20400000, LENGTH = 512M --------  */
  ram (wxa!ri) : ORIGIN = 0x00000000, LENGTH = 128K
  dccm (wxa!ri) : ORIGIN = 0xf0040000, LENGTH = 64K
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
    *(.gnu.linkonce.t.*)
    . = ALIGN(4);
  } >ram : ram_load


  .rodata :
  {
    *(.rdata)
    *(.rodata .rodata.*)
    *(.gnu.linkonce.r.*)
    . = ALIGN(4);
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
    . = ALIGN(8);
  } > ram : ram_load

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
  
  .sbss :
  {
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
    . = ALIGN(8);
  } > ram : ram_load
  
  . = ALIGN(4);
  PROVIDE( _edata = . );
  PROVIDE( edata = . );

  PROVIDE( _fbss = . );
  PROVIDE( __bss_start = . );
  
  .bss :
  {
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
}
