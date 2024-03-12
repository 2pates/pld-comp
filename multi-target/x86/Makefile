
PROG = ../example.c

default:run

compile: main.s

link: a.out

run: a.out
	@echo executing program:
	@./a.out ; echo 'return code:' $$?

a.out: main.s
	gcc -o a.out main.s

main.s: $(PROG)
	gcc -S -o main.s $(PROG)

clean:
	rm -f a.out main.s
