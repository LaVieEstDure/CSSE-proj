# CSSE2010 Makefile
# Author: Raghav Mishra
# Meant to be used with the Polulu Programmar v2
# and Atmega324A microcontrollers
# You will need installed:
#  avr-dude
#  avr-gcc toolchain
#  Polulu AVR configuration utility (optional, would need changes)

DEVICE=atmega324a
PROGRAMMER=-c stk500v2
PORT=-P $(shell pavr2cmd --prog-port)
AVRDUDE=sudo avrdude $(PROGRAMMER) $(PORT) -p m324pa -F
CFLAGS=-g -Os -Wall -mcall-prologues -std=gnu99 -mmcu=$(DEVICE)
LDFLAGS=-gc-sections -Wl,-relax
CC=avr-gcc
OBJ=project.o

all: project.hex

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

project.elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o project.elf 

disasm:	project.elf
	avr-objdump -d project.elf

project.hex: project.elf
	avr-objcopy -j .text -j .data -R .eeprom -O ihex project.elf project.hex

flash: all
	$(AVRDUDE) -U flash:w:project.hex:i

clean:
	rm -f project.o project.elf project.hex
