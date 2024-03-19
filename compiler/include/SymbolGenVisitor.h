#ifndef __SYMBOL_GEN_VISITOR__
#define __SYMBOL_GEN_VISITOR__

#include <map>
#include <vector>
#include <string>

#include "antlr4-runtime.h"
#include "ifccBaseVisitor.h"

#include "Error.h"

class VariableInfo {
public:
    VariableInfo() { defined = false; }
    VariableInfo(long int address_, int size_) : address(address_), size(size_), defined(true) {}
    VariableInfo(long int address_, int size_, bool defined_) : address(address_), size(size_), defined(defined_) {}
    long int address; // relative address of the pointer
    int size;         // size of the variable in octets
    bool defined;
    int block_id;       // block of the variable
};

class SymbolGenVisitor : public ifccBaseVisitor {
public:
    SymbolGenVisitor() : current_block(0), tmp_block_index(0), memory_offset(0), tmp_index(0) {}
    virtual antlrcpp::Any visitBlock(ifccParser::BlockContext* ctx) override;
    virtual antlrcpp::Any visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx) override;
    virtual antlrcpp::Any visitDeclare(ifccParser::DeclareContext* ctx) override;
    virtual antlrcpp::Any visitAssignment_stmt(ifccParser::Assignment_stmtContext* ctx) override;
    virtual antlrcpp::Any visitLvalue(ifccParser::LvalueContext* ctx) override;
    virtual antlrcpp::Any visitExpr_atom(ifccParser::Expr_atomContext* ctx) override;
    virtual antlrcpp::Any visitExpr_and(ifccParser::Expr_andContext* ctx) override;
    virtual antlrcpp::Any visitExpr_xor(ifccParser::Expr_xorContext* ctx) override;
    virtual antlrcpp::Any visitExpr_or(ifccParser::Expr_orContext* ctx) override;
    virtual antlrcpp::Any visitExpr_unaire(ifccParser::Expr_unaireContext* ctx) override;
    virtual antlrcpp::Any visitExpr_relational(ifccParser::Expr_relationalContext* ctx) override;
    virtual antlrcpp::Any visitExpr_equality(ifccParser::Expr_equalityContext* ctx) override;

    std::map<int, int> blocks; // id current block, id parent block
    int current_block;
    int tmp_block_index;

    std::map<std::string, VariableInfo> variables;
    long int memory_offset;
    int tmp_index;
    bool declaration_mode = false;
    std::vector<std::string> reserved_word{"if", "else", "switch", "case", // to optimize later
            "default", "break", "int", "float", "char",
            "double", "long", "for", "while", "do", "void",
            "goto", "auto", "signed", "const", "extern",
            "register", "unsigned", "return", "continue",
            "enum", "sizeof", "struct", "typedef", "union",
            "volatile"};

    
    int check_exist(ifccParser::Expr_atomContext* ctx);
    int check_exist(std::string varname);
};

#endif
