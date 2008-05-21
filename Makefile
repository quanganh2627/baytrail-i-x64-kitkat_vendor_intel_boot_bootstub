OBJ=bootstub.o head.o

all: bootstub

bootstub:bootstub.bin
	cat bootstub.bin /dev/zero | dd bs=4096 count=1 > $@

bootstub.bin:bootstub.elf
	objcopy -O binary -R .note -R .comment -S $< $@

bootstub.elf:bootstub.lds $(OBJ)
	ld -m elf_i386 -T bootstub.lds $(OBJ) -o $@

bootstub.o:bootstub.c
	gcc -c bootstub.c

head.o:head.S
	gcc -c head.S

clean:
	rm -rf *.o *.bin *.elf

targz:bootstub.tar.gz

bootstub.tar.gz:bootstub.c head.S
	git-archive --prefix=bootstub/ --format=tar HEAD | gzip -c > bootstub.tar.gz

.PHONY: all clean targz
