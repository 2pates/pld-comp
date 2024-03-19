# config.mk contains the paths to antlr4 etc.
# Each student should have a config.mk corresponding to her system.
# Examples are  ubuntu.mk, DI.mk, fedora.mk
# Then config.mk should be in the .gitignore of your project
include config/config.mk

CC=g++
CCFLAGS=-g -c -std=c++17 -I$(ANTLRINC) -Wno-attributes -DTRACE# -Wno-defaulted-function-deleted -Wno-unknown-warning-option
LDFLAGS=-g
T=

default: all

.PHONY:help
help : ## show this help
		@egrep -h '\s##\s' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "} \
		{printf "\033[36m%-20s\033[0m %s\n", $$1, $$2}'


all: ifcc

##########################################
# link together all pieces of our compiler 
OBJECTS=build/ifccBaseVisitor.o \
	build/ifccLexer.o \
	build/ifccVisitor.o \
	build/ifccParser.o \
	build/main.o \
	build/CodeGenVisitor.o \
	build/CodeGenVisitorExpr.o \
	build/SymbolGenVisitor.o \
	build/IR.o

ifcc: $(OBJECTS) ## compilation
	@mkdir -p build
	$(CC) $(LDFLAGS) build/*.o $(ANTLRLIB) -o build/ifcc

##########################################
# compile our hand-writen C++ code: main(), CodeGenVisitor, etc.
build/%.o: src/%.cpp generated/ifccParser.cpp
	@mkdir -p build
	$(CC) $(CCFLAGS) -MMD -o $@ $< -Iinclude -Igenerated

##########################################
# compile all the antlr-generated C++
build/%.o: generated/%.cpp
	@mkdir -p build
	$(CC) $(CCFLAGS) -MMD -o $@ $< 

# automagic dependency management: `gcc -MMD` generates all the .d files for us
-include build/*.d
build/%.d:

##########################################
# generate the C++ implementation of our Lexer/Parser/Visitor
generated/ifccLexer.cpp: generated/ifccParser.cpp
generated/ifccVisitor.cpp: generated/ifccParser.cpp
generated/ifccBaseVisitor.cpp: generated/ifccParser.cpp
generated/ifccParser.cpp: ifcc.g4
	@mkdir -p generated
	$(ANTLR) -visitor -no-listener -Dlanguage=Cpp -o generated ifcc.g4

# prevent automatic cleanup of "intermediate" files like ifccLexer.cpp etc
.PRECIOUS: generated/ifcc%.cpp   

##########################################
# view the parse tree in a graphical window

# Usage: `make gui FILE=path/to/your/file.c`
FILE ?= ../tests/testfiles/1_return42.c

gui: ## graphical interface
	@mkdir -p generated build
	$(ANTLR) -Dlanguage=Java -o generated ifcc.g4
	javac -cp $(ANTLRJAR) -d build generated/*.java
	java -cp $(ANTLRJAR):build org.antlr.v4.gui.TestRig ifcc axiom -gui $(FILE)

##########################################
# delete all machine-generated files
clean: ## clean repo
	rm -rf build generated
	rm -f build/ifcc
	@make -s -C ../tests clean


##########################################
# tests and verifications
tests: ## start all tests with python
	@make -s -C ../tests clean
	@make -s -C ../tests all



verify: ## check a specific compilation
	@read -p "Enter a number: " num; \
    files=$$(ls ../tests/testfiles/*/$$num\_*.c 2>/dev/null); \
    if [ -n "$$files" ]; then \
		./build/ifcc "$$files"; \
    else \
        echo "No file found for number $$num"; \
    fi


check:
	@make -s -C ../tests check



