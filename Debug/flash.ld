/*
 * Copyright (c) 2013 ELL-i co-op.
 *
 *
 * ELL-i software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * ELL-i software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with ELL-i software.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * This is the linker script file for ELL-i co-op STM32F051 based microcontroller boards.
 */

/* Memory Spaces Definitions */

ENTRY(Reset_Handler)

MEMORY
{
	FLASH	(rx)  : ORIGIN = 0x08000000, LENGTH = 1024K
  	RAM 	(rwx) : ORIGIN = 0x20000000, LENGTH = 128K
  	CCMRAM	(rwx) : ORIGIN = 0x10000000, LENGTH = 64K
}

_estack = 0x20020000;

/* Sections Definitions */

SECTIONS
{
    .text :
    {
        /* 
         * For Cortex-M devices, the text segment starts with the
         * interrupt vectors, including the address of the reset handler.
         * This is initialised in the XXX assembler file.
         */
	. = ALIGN(4);
        KEEP(*(.isr_vector))       /* Interrupt vector table */

        /* The program code is stored in the .text section, which goes to Flash */
	. = ALIGN(4);
        *(.text)                   /* Normal code */
        *(.text.*)                 /* Normal code in -ffunction-sections */
        *(.rodata)                 /* Read-only data (constants) */
        *(.rodata*)                /* Read-only data in -fdata-sections */

        /*
         * XXX
         */
        . = ALIGN(4);
        __peripheral_start = .;
        KEEP (*(.peripheral.RCC*))      /* Read-only RCC initialisation data */
        KEEP (*(SORT(.peripheral.*)))  /* Read-only peripheral initialisation data */
        __peripheral_end = .;

        /* 
         * The following sections are needed to jump from ARM code to Thumb
         * ocde and vice versa.  They are not needed in Cortex-M0, but they
         * don't harm either (other than confuse the reader...)
         */
        *(.glue_7)                 /* Glue code for jumping from ARM to Thumb */
        *(.glue_7t)                /* Glue code for jumping from Thumb to ARM */

        /*
         * .init, .preinit_array, and .init_array are sections supported by
         * the Linux linker and providing code and pointers to code that
         * is executed before the main program get started.  They are usually
         * used only in shared libraries.
         *
         * They are *not* used in the ELL-i runtime.  However, we are
         * keeping the sections here, checking that they are empty.
         */
        . = ALIGN(4);
        __init_start = .;
	KEEP (*(.init))
        . = ALIGN(4);
        KEEP (*(.preinit_array))
        . = ALIGN(4);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_end = .;

        ASSERT(__init_start == __init_end, "Non-zero-size .init, .preinit_array, or .init_array section.");

        /*
         * We do not Support C++ static constructors, either.
         */
        . = ALIGN(0x4);
        __ctor_begin = .;
        KEEP (*crtbegin.o(.ctors))       /* The initial dummy, empty entry */
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))         /* The last NULL entry */
        __ctor_end = .;

        ASSERT(__ctor_begin == __ctor_end, "C++ static constructors are not supported in this runtime.");

        /*
         * XXX Add here ELL-i read-only sections for peripheral initialisation
         */

        /*
         * Mark the end of the text section
         */
        . = ALIGN(4);
        _etext = .;
        _sidata = _etext;

    } > FLASH

    /* 
     * The initialized data section.
     *
     * The initialized data values need to be stored into the flash as the
     * the RAM contents are not preserved over resets.  The assembler startup
     * code copies the data into its proper location in RAM.  Hence, the 
     * addresses are linked as if this section was actually stored in RAM.
     */
    .data  : AT ( _sidata )
    {
	. = ALIGN(4);
        _sdata = . ;    /* Start of the data copied from flash to RAM */
        _data = . ;

        *(.data)
        *(.data.*)
        *(.RAMtext)

        . = ALIGN(4);
        _edata = . ;    /* End of the data copied from flash to RAM */
    } >RAM

    /*
     * The uninitialized data section. 
     *
     * All the global and static (i.e. non-automatic) variables that
     * are initialised as zero get assigned into this section.  This
     * section does not exist in the flash or binary, but is initiased
     * as zero by the assembler startup code.
     */
    .bss :
    {
	. = ALIGN(4);
        _sbss = .;      /* Start of the zero-initialized data */
        _bss = .;

        *(.bss)
        *(.bss.*)
        *(COMMON)       /* XXX remove and use -fno-common */
        /* See http://stackoverflow.com/questions/610682/bss-section-in-elf-file */

        . = ALIGN(4);
	_ebss = . ;     /* End of the zero-initialized data */
    } >RAM

    PROVIDE ( end = _ebss );
    PROVIDE ( _end = _ebss );

    __exidx_start = .;
    __exidx_end = .;

    /* 
     * Debugging information, stored in the ELF object but not
     * in the actual binary or hex file.
     *
     * Google for ELF and DWARF to find more information about them.
     *
     * At the moment (November 2013) these are not very useful for
     * us (the ELL-i co-op), as we don't have any working remote
     * debugging facilities, but they will be once we get there.
     */

    /* remove the debugging information from the standard libraries */
    /DISCARD/ :
    {
      *libc.a ( * )
      *libm.a ( * )
      *libgcc.a ( * )
    }

    /*
     * Traditional symbol table debugging sections.  
     */
    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment       0 : { *(.comment) }
    /* 
     * DWARF debug sections.
     *
     * Symbols in the DWARF debugging sections are relative to the beginning
     * of the section so we begin them at 0.  
     */

    /* DWARF 1 */
    .debug           0 : { *(.debug) }
    .line            0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo   0 : { *(.debug_srcinfo) }
    .debug_sfnames   0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges   0 : { *(.debug_aranges) }
    .debug_pubnames  0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info      0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev    0 : { *(.debug_abbrev) }
    .debug_line      0 : { *(.debug_line) }
    .debug_frame     0 : { *(.debug_frame) }
    .debug_str       0 : { *(.debug_str) }
    .debug_loc       0 : { *(.debug_loc) }
    .debug_macinfo   0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
}
