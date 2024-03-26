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
};

class SymbolGenVisitor : public ifccBaseVisitor {
public:
    SymbolGenVisitor() : memory_offset(0), tmp_index(0) {}
    virtual antlrcpp::Any visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx) override;
    virtual antlrcpp::Any visitDeclare(ifccParser::DeclareContext* ctx) override;
    virtual antlrcpp::Any visitAssignment_equal(ifccParser::Assignment_equalContext* ctx) override;
    virtual antlrcpp::Any visitAssignment_add(ifccParser::Assignment_addContext* ctx) override;
    virtual antlrcpp::Any visitAssignment_mult(ifccParser::Assignment_multContext* ctx) override;
    virtual antlrcpp::Any visitPre_incrementation(ifccParser::Pre_incrementationContext* ctx) override;
    virtual antlrcpp::Any visitPost_incrementation(ifccParser::Post_incrementationContext* ctx) override;
    virtual antlrcpp::Any visitSelection_if(ifccParser::Selection_ifContext* ctx) override;
    virtual antlrcpp::Any visitIteration_while(ifccParser::Iteration_whileContext* ctx) override;
    virtual antlrcpp::Any visitLvalue(ifccParser::LvalueContext* ctx) override;
    virtual antlrcpp::Any visitExpr_atom(ifccParser::Expr_atomContext* ctx) override;
    virtual antlrcpp::Any visitExpr_and(ifccParser::Expr_andContext* ctx) override;
    virtual antlrcpp::Any visitExpr_xor(ifccParser::Expr_xorContext* ctx) override;
    virtual antlrcpp::Any visitExpr_or(ifccParser::Expr_orContext* ctx) override;
    virtual antlrcpp::Any visitExpr_unaire(ifccParser::Expr_unaireContext* ctx) override;
    virtual antlrcpp::Any visitExpr_relational(ifccParser::Expr_relationalContext* ctx) override;
    virtual antlrcpp::Any visitExpr_equality(ifccParser::Expr_equalityContext* ctx) override;
    virtual antlrcpp::Any visitExpr_add(ifccParser::Expr_addContext* ctx) override;
    virtual antlrcpp::Any visitExpr_mult(ifccParser::Expr_multContext* ctx) override;
    virtual antlrcpp::Any visitExpr_parenthesis(ifccParser::Expr_parenthesisContext* ctx) override;
    virtual antlrcpp::Any visitExpr_lazy_and(ifccParser::Expr_lazy_andContext* ctx) override;
    virtual antlrcpp::Any visitExpr_lazy_or(ifccParser::Expr_lazy_orContext* ctx) override;

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
