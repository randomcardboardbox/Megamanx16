x16EMU = ../emulator/x16emu 
box16EMU = ../box16/Box16
ALL_ASM = $(wildcard src/*.asm) $(wildcard src/*.inc)
ALL_C = src/main.c
#$(wildcard src/*.c) $(wildcard src/*.h)

ALL_OBJS = $(patsubst %.c,%.o,$(wildcard src/*.c)) $(patsubst %.asm,%.obj,$(wildcard src/*.asm))

all: $(ALL_OBJS)
	cl65 -t cx16 -Osir -o x16_file_sys/MEGAMAN.PRG -Cl -C cx16-bank.cfg -m mega.mmap $(ALL_OBJS) src/zsound.lib
	rm -f src/*.mmap src/*.o src/*.obj src/*.s *.mmap

%.o: %.c
	cc65 -O -t cx16 -o $(patsubst %.o,%.s,$@) $<
	ca65 -t cx16 -o $@ $(patsubst %.o,%.s,$@)

%.obj: %.asm
	ca65 -t cx16 -o $@ $<

run:
	make
	cd x16_file_sys; \
	$(x16EMU) -run -prg MEGAMAN.PRG -quality nearest -scale 2 -debug

debug:
	make
	cd x16_file_sys; \
	$(box16EMU) -run -prg MEGAMAN.PRG -quality nearest -scale 2

clean:
	rm -f *.PRG *.mmap *.o *.obj *.s
	rm -f src/*.mmap src/*.o src/*.obj src/*.s *.mmap