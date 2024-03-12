#ifndef __SYMBOL_GEN_VISITOR__
#define __SYMBOL_GEN_VISITOR__

#include <map>

#include "antlr4-runtime.h"
#include "ifccBaseVisitor.h"

#include "Error.h"

class VariableInfo {
public:
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
    virtual antlrcpp::Any visitAssignment_stmt(ifccParser::Assignment_stmtContext* ctx) override;
    // virtual antlrcpp::Any visitExpr(ifccParser::ExprContext* ctx) override;
    // virtual antlrcpp::Any visitAtomic_expr(ifccParser::Atomic_exprContext* ctx) override;


    std::map<std::string, VariableInfo> variables;
    long int memory_offset;
    int tmp_index;

    int check_exist(ifccParser::Expr_atomContext* ctx);
    int check_exist(std::string varname);
};

#endif
