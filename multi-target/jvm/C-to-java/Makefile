compile: Example.class
disassemble: Example.j
run: compile
	java Example ; echo 'return code:' $$?

Example.class: Example.java
	javac Example.java

Example.j: Example.class
	java -cp ../tinapoc/bin/bcel-5.1.jar:../tinapoc/bin/java_cup.jar:../tinapoc/bin/tinapoc.jar dejasmin --dontprintoffsets Example.class > Example.j

clean:
	rm -f *.class *.j
