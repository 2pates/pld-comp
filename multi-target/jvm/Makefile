default: run

compile: Example.j

link: Example.class
 
run: Example.class
	java -noverify Example ; echo 'return code:' $$?

# here we cheat because we don't have a direct `.c` -> `.j` toolchain
Example.j:
	cd C-to-java ; make Example.j ; mv Example.j ..

Example.class: Example.j
	java -jar jasmin-2.4/jasmin.jar -d . Example.j


clean:
	rm -f Example.j Example.class
