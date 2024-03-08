#pragma once

#include "SymbolGenVisitor.h"
#include "antlr4-runtime.h"
#include "ifccBaseVisitor.h"

#include "Error.h"

class CodeGenVisitor : public ifccBaseVisitor {
public:
    CodeGenVisitor(std::map<std::string, VariableInfo> variables_) : variables(variables_), tmp_index(0) {}
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;
    virtual antlrcpp::Any visitInstruction(ifccParser::InstructionContext* ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext* ctx) override;
    virtual antlrcpp::Any visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx) override;
    virtual antlrcpp::Any visitAssignment_stmt(ifccParser::Assignment_stmtContext* ctx) override;
    virtual antlrcpp::Any visitRvalue(ifccParser::RvalueContext* ctx) override;
    virtual antlrcpp::Any visitLvalue(ifccParser::LvalueContext* ctx) override;

    std::map<std::string, VariableInfo> variables;
    int tmp_index;

    int push_stack(int source, int dest);
    int push_stack(std::string source, int dest, int size);
};
