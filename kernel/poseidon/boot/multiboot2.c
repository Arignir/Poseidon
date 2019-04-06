/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

#include <poseidon/boot/multiboot2.h>
#include <poseidon/memory.h>

/* The multiboot structure, filled by the bootloader */
struct multiboot_tag const *mb_tag = NULL;

/*
** Pointers to the most common multiboot tags.
**
** See `poseidon/boot/multiboot2.h` for their content.
*/

struct multiboot_tag_string const *mb_cmdline = NULL;
struct multiboot_tag_string const *mb_bootloader = NULL;
struct multiboot_tag_basic_meminfo const *mb_meminfo = NULL;
struct multiboot_tag_mmap const *mb_mmap = NULL;

/*
** Parse the multiboot structure to save pointers to the most common tags.
*/
void
multiboot_load(void)
{
	struct multiboot_tag const *tag;

	tag = mb_tag;

	if (!mb_tag) {
		return;
	}

	while (tag->type != MULTIBOOT_TAG_TYPE_END)
	{
		switch (tag->type)
		{
		case MULTIBOOT_TAG_TYPE_CMDLINE:
			mb_cmdline = (struct multiboot_tag_string const *)tag;
			break;
		case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
			mb_bootloader = (struct multiboot_tag_string const *)tag;
			break;
		case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
			mb_meminfo = (struct multiboot_tag_basic_meminfo const *)tag;
			break;
		case MULTIBOOT_TAG_TYPE_MMAP:
			mb_mmap = (struct multiboot_tag_mmap const *)tag;
			break;
		}

		// Each tag is aligned on the next 8-byte boundary.
		tag = (struct multiboot_tag const *)ALIGN(
			(uchar const *)tag + tag->size,
			8
		);
	}
}
