/* bootstub.c for loading protected mode kernel
 * Copyright (C) 2008 Alek Du <alek.du@intel.com>
 */

#include "types.h"
#include "bootstub.h"
#include "bootparam.h"

struct gdt_ptr {
        u16 len;
        u32 ptr;
} __attribute__((packed));

static void setup_gdt(void)
{
        static const u64 boot_gdt[] __attribute__((aligned(16))) = {
                /* CS: code, read/execute, 4 GB, base 0 */
                [GDT_ENTRY_BOOT_CS] = GDT_ENTRY(0xc09b, 0, 0xfffff),
                /* DS: data, read/write, 4 GB, base 0 */
                [GDT_ENTRY_BOOT_DS] = GDT_ENTRY(0xc093, 0, 0xfffff),
        };
        static struct gdt_ptr gdt;

        gdt.len = sizeof(boot_gdt)-1;
        gdt.ptr = (u32)&boot_gdt;

        asm volatile("lgdtl %0" : : "m" (gdt));
}

static void setup_idt(void)
{
        static const struct gdt_ptr null_idt = {0, 0};
        asm volatile("lidtl %0" : : "m" (null_idt));
}

static void *memcpy(void *dest, const void *src, size_t count)
{
        char *tmp = dest;
        const char *s = src;

        while (count--)
                *tmp++ = *s++;
        return dest;
}

static void *memset(void *s, int c, size_t count)
{
        char *xs = s;
 
        while (count--)
                *xs++ = c; 
        return s;
}

static size_t strnlen(const char *s, size_t maxlen)
{
        const char *es = s;
        while (*es && maxlen) {
                es++;
                maxlen--;
        }

        return (es - s);
}


static void setup_boot_params(struct boot_params *bp, struct setup_header *sh)
{
	memset(bp, 0, sizeof (struct boot_params));
	bp->screen_info.orig_video_mode = 1;
	bp->screen_info.orig_video_lines = 25;
	bp->screen_info.orig_video_cols = 80;
	bp->alt_mem_k = *(u32 *)MEMORY_SIZE_OFFSET*1024;
	memcpy(&bp->hdr, sh, sizeof (struct setup_header));
	bp->hdr.cmd_line_ptr = CMDLINE_OFFSET;
	bp->hdr.cmdline_size = strnlen((const char *)CMDLINE_OFFSET,256);
	bp->hdr.ramdisk_size = *(u32 *)INITRD_SIZE_OFFSET;
	bp->hdr.ramdisk_image = (bp->alt_mem_k*1024 - bp->hdr.ramdisk_size) & ~(0x0fff);
	memcpy((u8*)bp->hdr.ramdisk_image, (u8 *)BZIMAGE_OFFSET + *(u32 *)BZIMAGE_SIZE_OFFSET + *(u32 *)INITRD_SIZE_OFFSET,
		bp->hdr.ramdisk_size);
}

static int get_32bit_entry(unsigned char *ptr)
{
	while (1){
		if (*(u32 *)ptr == SETUP_SIGNATURE && *(u32 *)(ptr+4) == 0)
			break;
		ptr++;
	}
	ptr+=4;
	return (((unsigned int)ptr+511)/512)*512;
}

int main(void)
{
	setup_idt();
	setup_gdt();
	setup_boot_params((struct boot_params *)BOOT_PARAMS_OFFSET, 
		(struct setup_header *)SETUP_HEADER_OFFSET);
	return get_32bit_entry((unsigned char *)BZIMAGE_OFFSET);
}

