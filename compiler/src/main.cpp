#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include "antlr4-runtime.h"
#include "ifccBaseVisitor.h"
#include "ifccLexer.h"
#include "ifccParser.h"

#include "CodeGenVisitor.h"
#include "Error.h"
#include "IR.h"
#include "SymbolGenVisitor.h"

using namespace antlr4;
using namespace std;

int main(int argn, const char** argv) {
    stringstream in;
    if (argn == 2) {
        ifstream lecture(argv[1]);
        if (!lecture.good()) {
            cerr << "error: cannot read file: " << argv[1] << endl;
            exit(1);
        }
        in << lecture.rdbuf();
    } else {
        cerr << "usage: ifcc path/to/file.c" << endl;
        exit(1);
    }

    ANTLRInputStream input(in.str());

    ifccLexer lexer(&input);
    CommonTokenStream tokens(&lexer);

    tokens.fill();

    ifccParser parser(&tokens);
    tree::ParseTree* tree = parser.axiom();

    if (parser.getNumberOfSyntaxErrors() != 0) {
        cerr << "Error: syntax error during parsing" << endl;
        exit(1);
    }

    SymbolGenVisitor s;
    s.visit(tree);
    for (auto& var : s.variables) {
        if (var.first[0] != '#' && var.second.used == false) {
            string name = var.first.substr(0, var.first.find('#'));
            error("Variable '" + name + "' declared but not used");
            exit(UNUSED_VARIABLE);
        }
    }
    CFG cfg(s.variables, "entry_point");
    CodeGenVisitor v(&cfg, s.blocks);
    v.visit(tree);
    cfg.memoryUse = s.memory_offset;
    cfg.gen_asm(cout, Target::x86);
    cerr << EXIT_SUCCESS << endl;

    return EXIT_SUCCESS;
}
