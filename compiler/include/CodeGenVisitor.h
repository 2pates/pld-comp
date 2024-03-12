#pragma once

#include "SymbolGenVisitor.h"
#include "antlr4-runtime.h"
#include "ifccBaseVisitor.h"
#include <string.h>
#include "Error.h"

class CodeGenVisitor : public ifccBaseVisitor {
public:
    CodeGenVisitor(std::map<std::string, VariableInfo> variables_) : variables(variables_), tmp_index(0) {}
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;
    virtual antlrcpp::Any visitInstruction(ifccParser::InstructionContext* ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext* ctx) override;
    virtual antlrcpp::Any visitDeclare(ifccParser::DeclareContext* ctx) override;
    virtual antlrcpp::Any visitAssignment_stmt(ifccParser::Assignment_stmtContext* ctx) override;
    virtual antlrcpp::Any visitRvalue(ifccParser::RvalueContext* ctx) override;

    virtual antlrcpp::Any visitExpr_parenthesis(ifccParser::Expr_parenthesisContext* ctx) override;
    virtual antlrcpp::Any visitExpr_unaire(ifccParser::Expr_unaireContext* ctx) override;
    virtual antlrcpp::Any visitExpr_mult(ifccParser::Expr_multContext* ctx) override;
    virtual antlrcpp::Any visitExpr_add(ifccParser::Expr_addContext* ctx) override;
    virtual antlrcpp::Any visitExpr_relational(ifccParser::Expr_relationalContext* ctx) override;
    virtual antlrcpp::Any visitExpr_equality(ifccParser::Expr_equalityContext* ctx) override;
    virtual antlrcpp::Any visitExpr_and(ifccParser::Expr_andContext* ctx) override;
    virtual antlrcpp::Any visitExpr_xor(ifccParser::Expr_xorContext* ctx) override;
    virtual antlrcpp::Any visitExpr_or(ifccParser::Expr_orContext* ctx) override;
    virtual antlrcpp::Any visitExpr_lazy_and(ifccParser::Expr_lazy_andContext* ctx) override;
    virtual antlrcpp::Any visitExpr_lazy_or(ifccParser::Expr_lazy_orContext* ctx) override;
    virtual antlrcpp::Any visitExpr_atom(ifccParser::Expr_atomContext* ctx) override;

    std::map<std::string, VariableInfo> variables;
    int tmp_index;

    int push_stack(int source, int dest);
    int push_stack(std::string source, int dest, int size);

    int mov(std::string source, std::string dest, int size);
};

