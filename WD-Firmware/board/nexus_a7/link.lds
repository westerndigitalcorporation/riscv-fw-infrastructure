OUTPUT_ARCH( "riscv" )

ENTRY( _start )

MEMORY
{
  /* --------  flash (rxai!w) : ORIGIN = 0x20400000, LENGTH = 512M --------  */
  ram (wxa!ri) : ORIGIN = 0x00000000, LENGTH = 64K
}


SECTIONS
{
  __stack_size = DEFINED(__stack_size) ? __stack_size : 4K;

  .text.init	:
  {
    *(.text.init)
    . = ALIGN(8);
  } > ram

  .text           :
  {
    *(.text.unlikely .text.unlikely.*)
    *(.text.startup .text.startup.*)
    *(.text .text.*)
    *(.gnu.linkonce.t.*)
    . = ALIGN(4);
  } >ram


  .rodata         :
  {
    *(.rdata)
    *(.rodata .rodata.*)
    *(.gnu.linkonce.r.*)
    . = ALIGN(4);
  } > ram

  


  .lalign         :
  {
    . = ALIGN(4);
    PROVIDE( _data_lma = . );
  } > ram
  .dalign         :
  {
    . = ALIGN(4);
    PROVIDE( _data = . );
  } > ram
  .data          :
  {
    *(.data .data.*)
    *(.gnu.linkonce.d.*)
    . = ALIGN(8);
  } > ram

  .sdata	:
  {
    . = ALIGN(8);
    __global_pointer$ = . + 0x800;
    *(.sdata .sdata.*)
    *(.gnu.linkonce.s.*)
    . = ALIGN(8);
    *(.srodata .srodata.*)
   . = ALIGN(8);
  } > ram
  
  .sbss  :
  {
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
    . = ALIGN(8);
  } > ram 
  
  . = ALIGN(4);
  PROVIDE( _edata = . );
  PROVIDE( edata = . );

  PROVIDE( _fbss = . );
  PROVIDE( __bss_start = . );
  
  .bss :
  {
    *(.bss)
    . = ALIGN(8);
  } >ram

  _end = .;

  .stack :
  {
    _heap_end = .;
    . = . + __stack_size;
    _sp = .;
  } > ram
}
