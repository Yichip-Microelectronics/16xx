/******************************************************************************
 * @file     link_script.ld
 * @brief    GNU Linker Script
 * @version  V1.0.0
 ******************************************************************************/
OUTPUT_ARCH( "riscv" )

ENTRY( _start )

MEMORY
{
    flash         (rxai!w) : ORIGIN = 0x01020000, LENGTH = 512K
    retention_ram (wxa!ri) : ORIGIN = 0x10050, LENGTH = 8K-0x50
    ram           (wxa!ri) : ORIGIN = 0x12000, LENGTH = 57K
    ramcode       (rxai!w) : ORIGIN = 0x20400, LENGTH = 15k
}


SECTIONS
{
    .init           :
    {
        KEEP (*(SORT_NONE(.init)))
        . = ALIGN(4);
    } >flash AT>flash

    PROVIDE(_etext_ramcode = .);

    .ramcode          :
    {
        . = ALIGN(4);
        PROVIDE(_sramcode = .);
        KEEP(*(.vtable))
        KEEP(*trap_entry*.o)
        . = (. + 1023) / 1024 * 1024 ;
        KEEP(*trap_interrupt_gcc*.o)
        *(.ramcode*)
        *(._SECTION_RAM_CODE_DEFINE*)
        . = ALIGN(4);
        PROVIDE(_eramcode = .);
    } >ramcode AT>flash

    .text           :
    {
        . = ALIGN(4);
        *(.text.unlikely .text.unlikely.*)
        *(.text .text.*)
        *(.gnu.linkonce.t.*)
    } >flash AT>flash

    .rodata : ALIGN(4)
    {
        . = ALIGN(4);
        *(.rdata)
        *(.rodata .rodata.*)
        /* section information for initial. */
        . = ALIGN(4);
        __rt_init_start = .;
        KEEP(*(SORT(.rti_fn*)))
        __rt_init_end = .;
        /* section information for finsh shell */
        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;
        . = ALIGN(4);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;
        *(.gnu.linkonce.r.*)

    } >flash AT>flash

    .fini           :
    {
        KEEP (*(SORT_NONE(.fini)))
    } >flash AT>flash

    . = ALIGN(4);

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
    } >flash AT>flash

    .dtors          :
    {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
    } >flash AT>flash

    . = ALIGN(4);
    PROVIDE(_etext_data_retention = .);

    .data_retention          :
    {
        . = ALIGN(4);
        PROVIDE(_sdata_retention = .);
        _fixed = .;
        *(.fixed)
        *(lpm_retention)
        *(.__bdfunc__*)
        *(.__bdpara__*)
        *(.sbss.__malloc*)
        *(.sdata._impure_ptr)
        *(.data.impure_data)
        . = ALIGN(4);
        PROVIDE(_edata_retention = .);
    } >retention_ram AT>flash

    PROVIDE(_etext_data = (_etext_data_retention + (_edata_retention - _sdata_retention)));

    .data          :
    {
        . = ALIGN(4);
        PROVIDE(_sdata = .);

        *(.data .data.*)
        *(.gnu.linkonce.d.*)
        . = ALIGN(8);
        PROVIDE( __global_pointer$ = . + 0x800 );
        *(.sdata .sdata.* .sdata*)
        *(.gnu.linkonce.s.*)
        . = ALIGN(8);
        *(.srodata.cst16)
        *(.srodata.cst8)
        *(.srodata.cst4)
        *(.srodata.cst2)
        *(.srodata .srodata.*)

        . = ALIGN(4);
        PROVIDE( _edata = . );
        PROVIDE( edata = . );
    } >ram AT>flash

    PROVIDE( _fbss = . );
    PROVIDE( _sbss = . );
    PROVIDE( __bss_start = . );

    .bss            :
    {
        *(.sbss*)
        *(.gnu.linkonce.sb.*)
        *(.bss .bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)
        . = ALIGN(4);
    } >ram AT>ram

    . = ALIGN(8);
    PROVIDE( _end = . );
    PROVIDE( end = . );
    PROVIDE( _ebss = . );
    PROVIDE( _heap_start = . );
    PROVIDE( _heap_end = _heap_start + __heap_size);
    PROVIDE( _sp = ORIGIN(ram) + LENGTH(ram));
    ASSERT(_heap_end < _sp, "ram size overflow!")
}
