
PROG = ../example.c

default:run

compile: main.s

link: main.elf

run: main.elf
	qemu-arm -L /usr/arm-linux-gnu/sys-root main.elf ; echo 'return code:' $$?

main.s: $(PROG)
	arm-linux-gnu-gcc -S -o main.s $(PROG)

main.elf: main.s
	arm-linux-gnu-gcc -o main.elf main.s

clean:
	rm -f main.elf *.s objdump*.txt mspdebug-log.txt
