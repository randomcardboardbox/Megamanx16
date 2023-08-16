CC = cc65/bin
x16EMU = ../emulator/x16emu 
ALL_ASM = $(wildcard src/*.asm) $(wildcard src/*.inc)
ALL_C = src/main.c
#$(wildcard src/*.c) $(wildcard src/*.h)

ALL_OBJS = $(patsubst %.c,%.o,$(wildcard src/*.c)) $(patsubst %.asm,%.obj,$(wildcard src/*.asm))

all: $(ALL_OBJS)
	$(CC)/cl65 -t cx16 -o x16_file_sys/MEGAMAN.PRG -m mega.mmap $(ALL_OBJS)
	rm -f src/*.mmap src/*.o src/*.obj src/*.s *.mmap

%.o: %.c
	$(CC)/cc65 -O -t cx16 -o $(patsubst %.o,%.s,$@) $<
	$(CC)/ca65 -t cx16 -o $@ $(patsubst %.o,%.s,$@)

%.obj: %.asm
	$(CC)/ca65 -t cx16 -o $@ $<

run:
	make
	cd x16_file_sys; \
	$(x16EMU) -run -prg MEGAMAN.PRG -quality nearest -scale 2 -debug

clean:
	rm -f *.PRG *.mmap *.o *.obj *.s
	rm -f src/*.mmap src/*.o src/*.obj src/*.s *.mmap