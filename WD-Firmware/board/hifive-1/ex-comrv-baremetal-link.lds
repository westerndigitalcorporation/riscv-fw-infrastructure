OUTPUT_ARCH( "riscv" )

ENTRY( _start )

MEMORY
{
  flash (rxai!w)   : ORIGIN = 0x20400000, LENGTH = 512M
  /*ovldata (rxai!w) : ORIGIN = 0x26800000, LENGTH = 12M*/
  ram (wxa!ri)     : ORIGIN = 0x80000000, LENGTH = 14K
  ovl (wxa!ri)     : ORIGIN = 0x80003800, LENGTH = 2K
  /* Temporary location for storing overlay functions. This is needed for the
     linker to place functions, and as the addresses of functions in debuginfo,
     but is never loaded onto the system. */
  ovltmp           : ORIGIN = 0xF0000000, LENGTH = 0x10000
}

PHDRS
{
  flash PT_LOAD;
  ram_init PT_LOAD;
  ram PT_NULL;
  ovl PT_NULL;
}

SECTIONS
{
  __stack_size = DEFINED(__stack_size) ? __stack_size : 2K;

  .init           :
  {
    KEEP (*(SORT_NONE(.init)))
  } >flash AT>flash :flash

  /* Placeholder location for overlay functions. Marked as not allocated by the
     linker. This must live ahead of the .text declaration */
  .ovlallfns      : { *(.text.ovlfn.*) } > ovltmp

  .text           :
  {
    *(.text.unlikely .text.unlikely.*)
    *(.text.startup .text.startup.*)
    *(.text .text.*)
    *(.gnu.linkonce.t.*)
  } >flash AT>flash :flash

  .fini           :
  {
    KEEP (*(SORT_NONE(.fini)))
  } >flash AT>flash :flash

  PROVIDE (__etext = .);
  PROVIDE (_etext = .);
  PROVIDE (etext = .);

  .rodata         :
  {
    *(.rdata)
    *(.rodata .rodata.*)
    *(.gnu.linkonce.r.*)
  } >flash AT>flash :flash

  /* Overlay data */
  .ovlgrp.0       : {. = ALIGN(512); PROVIDE (OVERLAY_START_OF_OVERLAYS = .); __OVERLAY_START__ = .;} >flash AT>flash :flash
  .ovlgrp.1       : {} >flash AT>flash :flash
  .ovlgrp.2       : {} >flash AT>flash :flash
  .ovlgrp.3       : {} >flash AT>flash :flash

  /* Overlay offset table */
  .ovlgrptbl      : {
    overlayOffsetTable = .;
    PROVIDE (OVERLAY_GROUP_TABLE = .);
    *(.ovlgrptbl)
  } >flash AT>flash :flash
  /* Overlay multi group table */
  .ovlmultigroup  : {
    overlayMultiGroupTokensTable = .;
    PROVIDE (OVERLAY_MULTI_GROUP_TABLE = .);
    *(.ovlmultigroup)
  } >flash AT>flash :flash

  . = ALIGN(4);

  .preinit_array  :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >flash AT>flash :flash

  .init_array     :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >flash AT>flash :flash

  .fini_array     :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >flash AT>flash :flash

  .ctors          :
  {
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    /* We don't want to include the .ctor section from
       the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
  } >flash AT>flash :flash

  .dtors          :
  {
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
  } >flash AT>flash :flash

  .lalign         :
  {
    . = ALIGN(4);
    PROVIDE( _data_lma = . );
  } >flash AT>flash :flash

  .dalign         :
  {
    . = ALIGN(4);
    PROVIDE( _data = . );
  } >ram AT>flash :ram_init

  .data          :
  {
    *(.data .data.*)
    *(.gnu.linkonce.d.*)
    . = ALIGN(8);
    PROVIDE( __global_pointer$ = . + 0x800 );
    *(.sdata .sdata.*)
    *(.gnu.linkonce.s.*)
    . = ALIGN(8);
    *(.srodata.cst16)
    *(.srodata.cst8)
    *(.srodata.cst4)
    *(.srodata.cst2)
    *(.srodata .srodata.*)
  } >ram AT>flash :ram_init

  . = ALIGN(4);
  PROVIDE( _edata = . );
  PROVIDE( edata = . );

  PROVIDE( _fbss = . );
  PROVIDE( __bss_start = . );
  .bss            :
  {
    *(.sbss*)
    *(.gnu.linkonce.sb.*)
    *(.bss .bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN(4);
  } >ram AT>ram :ram

  . = ALIGN(8);
  PROVIDE( _end = . );
  PROVIDE( end = . );

  .stack ORIGIN(ram) + LENGTH(ram) - __stack_size :
  {
    PROVIDE( _heap_end = . );
    . = __stack_size;
    PROVIDE( _sp = . );
  } >ram AT>ram :ram

  .overlay_sec ORIGIN(ovl) :
  {
    __OVERLAY_SEC_START__ = .;
    . = 1K;
    __OVERLAY_SEC_END__ = .;
  }  >ovl AT>ovl :ovl
  
  .ovl_stack ORIGIN(ovl) + LENGTH(ovl) - 1K :
  {
    __OVERLAY_STACK_START__ = .;
    . = 1K;
    __OVERLAY_STACK_END__ = .;
  }  >ovl AT>ovl :ovl
}
