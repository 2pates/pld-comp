#include "SymbolGenVisitor.h"
#include "Error.h"



antlrcpp::Any SymbolGenVisitor::visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx) {
    if (ctx->TYPE()->getText() == "int") {
        visit(ctx->declare());
    }
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitDeclare(ifccParser::DeclareContext* ctx) {
    if (ctx->lvalue() != nullptr) {
        std::string name = ctx->lvalue()->getText();
        if (check_exist(name) != 0) {
            memory_offset -= 4; // decrement index first !
            VariableInfo var(memory_offset, 4, false);
            variables.insert({name, var});

            std::cerr << "Declaration: " << name << " (address " << var.address << ")"
                    << std::endl;
        } else {
            std::cerr << "Already used name" << std::endl;
            return DOUBLE_DECLARATION;
        }
    } else if (ctx->assignment_stmt() != nullptr) {
        visit(ctx->assignment_stmt());
    }
    if (ctx->declare() != nullptr) {
        visit(ctx->declare());
    }
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitAssignment_stmt(ifccParser::Assignment_stmtContext* ctx) {
    //visit(ctx->lvalue());
    if (variables.find(ctx->lvalue()->getText()) != variables.end()) {
        visit(ctx->rvalue());
        std::cerr << "Affectation: " << ctx->lvalue()->getText() << " = " << ctx->rvalue()->getText() << std::endl;
        return 0;
    }
    std::cerr << "Error: undeclared variable " << ctx->lvalue()->getText() << std::endl;
    return UNDECLARED; // undeclared variable affectation
}

antlrcpp::Any SymbolGenVisitor::visitExpr_atom(ifccParser::Expr_atomContext* ctx)
{
    debug("expr_atom symbol");
    if (ctx->CONST() != nullptr) {
        memory_offset -= 4;
        tmp_index++;
        variables.insert({"#tmp"+std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    }
    return 0;
}


antlrcpp::Any SymbolGenVisitor::visitExpr_and(ifccParser::Expr_andContext* ctx) {
    visit(ctx->expr()[0]);
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp"+std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    visit(ctx->expr()[1]);
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_xor(ifccParser::Expr_xorContext* ctx) {
    visit(ctx->expr()[0]);
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp"+std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    visit(ctx->expr()[1]);
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_or(ifccParser::Expr_orContext* ctx) {
    visit(ctx->expr()[0]);
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp"+std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    visit(ctx->expr()[1]);
    return 0;
}

int SymbolGenVisitor::check_exist(ifccParser::Expr_atomContext* ctx) {
    if (ctx->CONST())
        return EXIST;
    else if (variables.find(ctx->VARNAME()->getText()) != variables.end())
        return EXIST;
    else
        return UNDECLARED;
}

int SymbolGenVisitor::check_exist(std::string varname) {
    if (variables.find(varname) != variables.end())
        return EXIST;
    else
        return UNDECLARED;
}


