#ifndef __SYMBOL_GEN_VISITOR__
#define __SYMBOL_GEN_VISITOR__

#include <string>
#include <unordered_map>
#include <vector>

#include "antlr4-runtime.h"
#include "ifccBaseVisitor.h"

#include "Error.h"

/**
 * @brief Class representing information about a variable. It gives temporary and permenant variables adresses and checks for variable declaration and use errors.
 */
class VariableInfo {
public:
    VariableInfo() {
        defined = false;
        used = false;
    }
    VariableInfo(long int address_, int size_) : address(address_), size(size_), defined(true), used(false) {}
    VariableInfo(long int address_, int size_, bool defined_) : address(address_), size(size_), defined(defined_), used(false) {}
    long int address; ///< Relative address of the pointer.
    int size;         ///< Size of the variable in octets.
    bool defined;     ///< Flag indicating if the variable is defined.
    bool used;        ///< Flag indicating if the variable is used.
};

/**
 * @brief Visitor class for generating symbols.
 */
class SymbolGenVisitor : public ifccBaseVisitor {
public:
    /**
     * @brief Constructor for SymbolGenVisitor class.
     */
    SymbolGenVisitor() : current_block(0), tmp_block_index(0), memory_offset(0), tmp_index(0) { blocks.insert({0, -1}); }

    // Visit functions for different grammar rules
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;
    virtual antlrcpp::Any visitBlock(ifccParser::BlockContext* ctx) override;
    virtual antlrcpp::Any visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx) override;
    virtual antlrcpp::Any visitAssignment_equal(ifccParser::Assignment_equalContext* ctx) override;
    virtual antlrcpp::Any visitAssignment_add(ifccParser::Assignment_addContext* ctx) override;
    virtual antlrcpp::Any visitAssignment_mult(ifccParser::Assignment_multContext* ctx) override;
    virtual antlrcpp::Any visitPre_incrementation(ifccParser::Pre_incrementationContext* ctx) override;
    virtual antlrcpp::Any visitPost_incrementation(ifccParser::Post_incrementationContext* ctx) override;
    virtual antlrcpp::Any visitSelection_if(ifccParser::Selection_ifContext* ctx) override;
    virtual antlrcpp::Any visitIteration_while(ifccParser::Iteration_whileContext* ctx) override;
    virtual antlrcpp::Any visitLvalue(ifccParser::LvalueContext* ctx) override;
    virtual antlrcpp::Any visitExpr_atom(ifccParser::Expr_atomContext* ctx) override;
    virtual antlrcpp::Any visitExpr_mult(ifccParser::Expr_multContext* ctx) override;
    virtual antlrcpp::Any visitExpr_and(ifccParser::Expr_andContext* ctx) override;
    virtual antlrcpp::Any visitExpr_xor(ifccParser::Expr_xorContext* ctx) override;
    virtual antlrcpp::Any visitExpr_or(ifccParser::Expr_orContext* ctx) override;
    virtual antlrcpp::Any visitExpr_unaire(ifccParser::Expr_unaireContext* ctx) override;
    virtual antlrcpp::Any visitExpr_relational(ifccParser::Expr_relationalContext* ctx) override;
    virtual antlrcpp::Any visitExpr_equality(ifccParser::Expr_equalityContext* ctx) override;
    virtual antlrcpp::Any visitExpr_add(ifccParser::Expr_addContext* ctx) override;
    virtual antlrcpp::Any visitExpr_function(ifccParser::Expr_functionContext* ctx) override;
    virtual antlrcpp::Any visitExpr_parenthesis(ifccParser::Expr_parenthesisContext* ctx) override;
    virtual antlrcpp::Any visitFunction_call(ifccParser::Function_callContext* ctx) override;
    virtual antlrcpp::Any visitExpr_assignment(ifccParser::Expr_assignmentContext* ctx) override;
    virtual antlrcpp::Any visitDeclare_only_stmt(ifccParser::Declare_only_stmtContext* ctx) override;
    virtual antlrcpp::Any visitExpr_lazy_and(ifccParser::Expr_lazy_andContext* ctx) override;
    virtual antlrcpp::Any visitExpr_lazy_or(ifccParser::Expr_lazy_orContext* ctx) override;
    virtual antlrcpp::Any visitFunction_def(ifccParser::Function_defContext* ctx) override;

    /**
     * @brief Generate a unique variable name.
     *
     * @param varname The variable name to make unique.
     * @return A unique variable name.
     */
    std::string get_unique_var_name(std::string varname);

    std::string currentFunction = "";                        ///< Name of the current function.
    std::unordered_map<int, int> blocks;                     ///< Mapping of block IDs to their parent block IDs.
    int current_block;                                       ///< ID of the current block.
    int tmp_block_index;                                     ///< Index for temporary block IDs.
    std::unordered_map<std::string, VariableInfo> variables; ///< Mapping of variable names to their information.
    long int memory_offset;                                  ///< Memory offset for variable addresses.
    int tmp_index;                                           ///< Index for temporary variable names.
    bool declaration_mode = false;                           ///< Flag indicating if in declaration mode.
    std::vector<std::string> reserved_word{                  ///< List of reserved words.
                                           "if",   "else",   "switch", "case",    "default",  "break",    "int",    "float",
                                           "char", "double", "long",   "for",     "while",    "do",       "void",   "goto",
                                           "auto", "signed", "const",  "extern",  "register", "unsigned", "return", "continue",
                                           "enum", "sizeof", "struct", "typedef", "union",    "volatile", "NULL"};

    /**
     * @brief Check if a variable exists in the current block.
     *
     * @param varname The variable name to check.
     * @return GOOD if the variable exists, UNDECLARED otherwise. (see Error.h to match values)
     */
    int check_exist_in_current_block(std::string varname);

    /**
     * @brief Check if a variable exists in the current or parent block.
     *
     * @param varname The variable name to check.
     * @return GOOD if the variable exists, UNDECLARED otherwise. (see Error.h to match values)
     */
    int check_exist_in_current_or_parent_block(std::string varname);

    /**
     * @brief Generate a new temporary variable name.
     *
     * @return A new temporary variable name.
     */
    std::string get_new_tmp_varname();

    /**
     * @brief Create a unique variable name.
     *
     * @param name The base name for the variable.
     * @return A unique variable name.
     */
    std::string create_unique_var_name(std::string name);

    bool inExpr = false; ///< Flag indicating if inside an expression.
};

#endif
