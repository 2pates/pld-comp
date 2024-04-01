#pragma once

#include "Error.h"
#include "IR.h"
#include "SymbolGenVisitor.h"
#include "antlr4-runtime.h"
#include "ifccBaseVisitor.h"
#include <string.h>

class CodeGenVisitor : public ifccBaseVisitor {
public:
    CodeGenVisitor(CFG* cfg_, std::unordered_map<int, int>& blocks_)
        : current_block(0), tmp_block_index(0), blocks(blocks_), tmp_index(0), cfg(cfg_), variables(cfg->variables),
          declaration_mode(false) {}
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;
    virtual antlrcpp::Any visitBlock(ifccParser::BlockContext* ctx) override;
    virtual antlrcpp::Any visitInstruction(ifccParser::InstructionContext* ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext* ctx) override;
    virtual antlrcpp::Any visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx) override;
    virtual antlrcpp::Any visitDeclare(ifccParser::DeclareContext* ctx) override;
    virtual antlrcpp::Any visitAssignment_stmt(ifccParser::Assignment_stmtContext* ctx) override;
    virtual antlrcpp::Any visitRvalue(ifccParser::RvalueContext* ctx) override;
    virtual antlrcpp::Any visitExpr_parenthesis(ifccParser::Expr_parenthesisContext* ctx) override;
    virtual antlrcpp::Any visitExpr_function(ifccParser::Expr_functionContext* ctx) override;
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
    virtual antlrcpp::Any visitFunction_call(ifccParser::Function_callContext *ctx) override;
    virtual antlrcpp::Any visitFunction_def(ifccParser::Function_defContext *ctx) override;
    virtual antlrcpp::Any visitSelection_if(ifccParser::Selection_ifContext* ctx) override;
    virtual antlrcpp::Any visitIteration_while(ifccParser::Iteration_whileContext* ctx) override;
    virtual antlrcpp::Any visitExpr_assignment(ifccParser::Expr_assignmentContext* ctx) override;
    virtual antlrcpp::Any visitReturn_stmt_fct(ifccParser::Return_stmt_fctContext* ctx) override;

    virtual antlrcpp::Any visitDeclare_only_stmt(ifccParser::Declare_only_stmtContext* ctx) override;

    std::string get_unique_var_name(std::string varname);

    int current_block;
    int tmp_block_index;
    std::unordered_map<int, int> blocks; // id current block, id parent block

    int tmp_index;
    CFG* cfg;
    std::unordered_map<std::string, VariableInfo>& variables;
    bool declaration_mode;
    int varInFunctionDef=0;
    std::string repList[6]={"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
};
