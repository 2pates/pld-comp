#ifndef __SYMBOL_GEN_VISITOR__
#define __SYMBOL_GEN_VISITOR__

#include <string>
#include <unordered_map>
#include <vector>

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
    SymbolGenVisitor() : current_block(0), tmp_block_index(0), memory_offset(0), tmp_index(0) {}
    virtual antlrcpp::Any visitBlock(ifccParser::BlockContext* ctx) override;
    virtual antlrcpp::Any visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx) override;
    //virtual antlrcpp::Any visitDeclare(ifccParser::DeclareContext* ctx) override;
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
    virtual antlrcpp::Any visitFunction_def(ifccParser::Function_defContext *ctx) override;

    std::string currentFunction="";
    std::unordered_map<int, int> blocks; // id current block, id parent block
    int current_block;
    int tmp_block_index;
 
    std::unordered_map<std::string, VariableInfo> variables;
    long int memory_offset;
    int tmp_index;
    bool declaration_mode = false;
    std::vector<std::string> reserved_word{"if",      "else",   "switch",   "case", // to optimize later
                                           "default", "break",  "int",      "float",    "char",    "double",   "long",
                                           "for",     "while",  "do",       "void",     "goto",    "auto",     "signed",
                                           "const",   "extern", "register", "unsigned", "return",  "continue", "enum",
                                           "sizeof",  "struct", "typedef",  "union",    "volatile",
                                           "NULL" // manually added
                                           };

    int check_exist_in_current_block(std::string varname);
    int check_exist_in_current_or_parent_block(std::string varname);
    std::string get_new_tmp_varname();
    std::string create_unique_var_name(std::string name);
};

#endif
