#pragma once

#include "Error.h"
#include "IR.h"
#include "SymbolGenVisitor.h"
#include "antlr4-runtime.h"
#include "ifccBaseVisitor.h"
#include <string.h>

/**
 * @brief Visitor class for generating code. Has two cpp one for expr and one for the rest, just for readability. Creates IRInst that are
 * turned into code by IR.
 */
class CodeGenVisitor : public ifccBaseVisitor {
public:
    /**
     * @brief Constructor for CodeGenVisitor class.
     *
     * @param cfg_ Pointer to the Control Flow Graph.
     * @param blocks_ Mapping of block IDs to their parent block IDs.
     */
    CodeGenVisitor(CFG* cfg_, std::unordered_map<int, int>& blocks_)
        : current_block(0), tmp_block_index(0), blocks(blocks_), tmp_index(0), cfg(cfg_), variables(cfg->variables),
          declaration_mode(false) {}

    // Visit functions for different grammar rules
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;
    virtual antlrcpp::Any visitBlock(ifccParser::BlockContext* ctx) override;
    virtual antlrcpp::Any visitInstruction(ifccParser::InstructionContext* ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext* ctx) override;
    virtual antlrcpp::Any visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx) override;
    virtual antlrcpp::Any visitDeclare_only_stmt(ifccParser::Declare_only_stmtContext* ctx) override;
    virtual antlrcpp::Any visitAssignment_equal(ifccParser::Assignment_equalContext* ctx) override;
    virtual antlrcpp::Any visitAssignment_add(ifccParser::Assignment_addContext* ctx) override;
    virtual antlrcpp::Any visitAssignment_mult(ifccParser::Assignment_multContext* ctx) override;
    virtual antlrcpp::Any visitPre_incrementation(ifccParser::Pre_incrementationContext* ctx) override;
    virtual antlrcpp::Any visitPost_incrementation(ifccParser::Post_incrementationContext* ctx) override;
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
    virtual antlrcpp::Any visitFunction_call(ifccParser::Function_callContext* ctx) override;
    virtual antlrcpp::Any visitFunction_def(ifccParser::Function_defContext* ctx) override;
    virtual antlrcpp::Any visitSelection_if(ifccParser::Selection_ifContext* ctx) override;
    virtual antlrcpp::Any visitIteration_while(ifccParser::Iteration_whileContext* ctx) override;
    virtual antlrcpp::Any visitExpr_assignment(ifccParser::Expr_assignmentContext* ctx) override;
    virtual antlrcpp::Any visitJump_break(ifccParser::Jump_breakContext* ctx) override;
    virtual antlrcpp::Any visitJump_continue(ifccParser::Jump_continueContext* ctx) override;

    /**
     * @brief Generate a unique variable name.
     *
     * @param varname The variable name to make unique.
     * @return A unique variable name.
     */
    std::string get_unique_var_name(std::string varname);

    int current_block;                   ///< ID of the current block.
    int tmp_block_index;                 ///< Index for temporary block IDs.
    std::unordered_map<int, int> blocks; ///< Mapping of block IDs to their parent block IDs.

    int tmp_index;                                            ///< Index for temporary variable names.
    CFG* cfg;                                                 ///< Pointer to the Control Flow Graph.
    std::unordered_map<std::string, VariableInfo>& variables; ///< Mapping of variable names to their information.
    bool declaration_mode;                                    ///< Flag indicating if in declaration mode.
    bool inmain = false;                                      ///< Flag indicating if inside main function.
    int varInFunctionDef = 0;                                 ///< Number of variables in a function definition.
    std::string currentFunction = "";                         ///< Name of the current function.
};
