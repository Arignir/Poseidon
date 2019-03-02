/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

#include <poseidon/kconfig.h>
#include <poseidon/boot/multiboot2.h>

.section .multiboot
mb_header_start:
	.long MULTIBOOT2_HEADER_MAGIC			// magic number
	.long 0						// architecture (x86 PM)
	.long mb_header_end - mb_header_start		// header length
	// checksum
	.long 0x100000000 - (MULTIBOOT2_HEADER_MAGIC + (mb_header_end - mb_header_start))

	// Graphic mode
#if KCONFIG_MULTIBOOT_FRAMEBUFFER
	.align 8
	.short MULTIBOOT_HEADER_TAG_FRAMEBUFFER 	// type
	.short 0					// flags
	.long 20					// size
	.long 1024					// width
	.long 768					// height
	.long 32					// depth
#endif

	// Module alignment
	.align 8
	.short MULTIBOOT_HEADER_TAG_MODULE_ALIGN	// type
	.short 0					// flags
	.long 8						// size

	// Required end tag
	.align 8
	.short MULTIBOOT_HEADER_TAG_END			// type
	.short 0					// flags
	.long 8						// size
mb_header_end:
