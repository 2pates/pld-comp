#ifndef __SYMBOL_GEN_VISITOR__
#define __SYMBOL_GEN_VISITOR__

#include <map>
#include <string>

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

class VariableInfo {
public:
    VariableInfo(long int address_, int size_) : address(address_), size(size_) {}
    long int address; // relative address of the pointer
    int size;         // size of the variable in octets
};

class SymbolGenVisitor : public ifccBaseVisitor {
public:
    SymbolGenVisitor() : current_index(0) {}
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;
    virtual antlrcpp::Any visitInstruction(ifccParser::InstructionContext* ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext* ctx) override;
    virtual antlrcpp::Any visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx) override;
    virtual antlrcpp::Any visitAssignment_stmt(ifccParser::Assignment_stmtContext* ctx) override;
    virtual antlrcpp::Any visitRvalue(ifccParser::RvalueContext* ctx) override;
    virtual antlrcpp::Any visitLvalue(ifccParser::LvalueContext* ctx) override;

    std::map<std::string, VariableInfo> variables;
    long int current_index;
};

#endif
