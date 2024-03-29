/* linux/arch/arm/mach-s5p6442/include/mach/memory.h
 *
 * Copyright 2008 Openmoko, Inc.
 * Copyright 2008 Simtec Electronics
 *      Ben Dooks <ben@simtec.co.uk>
 *      http://armlinux.simtec.co.uk/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __ASM_ARCH_MEMORY_H
#define __ASM_ARCH_MEMORY_H

#define PHYS_OFFSET_DDR			UL(0x20000000)
#define PHYS_SIZE_DDR_128M		(128 * 1024 * 1024)
#define PHYS_SIZE_DDR_256M		(256 * 1024 * 1024)
#define PHYS_OFFSET_ONEDRAM		UL(0x40000000)
#define PHYS_SIZE_ONEDRAM		(80* 1024 * 1024)
#define PHYS_OFFSET			PHYS_OFFSET_DDR

#define NODE_MEM_SIZE_BITS 29

#define CONSISTENT_DMA_SIZE	SZ_8M

#endif
