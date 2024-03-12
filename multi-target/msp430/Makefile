
PROG = ../example.c

default:run

compile: main.s

link: firmware.elf

run: firmware.elf
# step 1: execute the program in the interpreter
	@echo executing program:
	@mspdebug -q --embedded  sim "simio add console c" "prog firmware.elf" "step 10000" > mspdebug-log.txt
# step 2: extract standard output
	@cat mspdebug-log.txt |\
		head -n $$((-1 + $$(grep -n '^:    ( PC:' mspdebug-log.txt | cut -d: -f1) )) |\
		tail -n +$$( grep -n '^:Done, ' mspdebug-log.txt | cut -d: -f1 ) |\
		cut -d: -f2-
# step 3: extract return value from main
	@echo return code:
	@cat mspdebug-log.txt | grep '^:    ( R3:' | cut -b53-58 | xargs -I{} printf '%d\n' 0x{}

main.s: $(PROG)
	msp430-gcc -S -mmcu=msp430g2553 -o main.s $(PROG)

bsp.s: bsp.c
	msp430-gcc -S -mmcu=msp430g2553 -o bsp.s bsp.c

firmware.elf: main.s bsp.s
	msp430-gcc -mmcu=msp430g2553 -o firmware.elf main.s bsp.s
	msp430-objdump -zhD firmware.elf > objdump-msp430.txt

clean:
	rm -f firmware.elf *.s objdump*.txt mspdebug-log.txt
