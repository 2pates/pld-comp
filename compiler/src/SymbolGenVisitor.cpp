#include "SymbolGenVisitor.h"

antlrcpp::Any SymbolGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext* ctx) { return 0; }

antlrcpp::Any SymbolGenVisitor::visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx) {
    if (ctx->TYPE()->getText() == "int") {
        std::string name = ctx->lvalue()->getText();
        if (check_exist(name) != 0) {
            memory_offset -= 4; // decrement index first !
            VariableInfo var(memory_offset, 4, false);
            variables.insert({name, var});

            std::cerr << "Declaration: " << ctx->TYPE()->getText() << " " << name << " (address " << var.address << ")"
                      << std::endl;
        } else {
            std::cerr << "Already used name" << std::endl;
            return DOUBLE_DECLARATION;
        }
    }
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitAssignment_stmt(ifccParser::Assignment_stmtContext* ctx) {
    visit(ctx->lvalue());
    if (variables.find(ctx->lvalue()->getText()) != variables.end()) {
        visit(ctx->rvalue());
        std::cerr << "Affectation: " << ctx->lvalue()->getText() << " = " << ctx->rvalue()->getText() << std::endl;
        return 0;
    }
    std::cerr << "Error: undeclared variable " << ctx->lvalue()->getText() << std::endl;
    return UNDECLARED; // undeclared variable affectation
}

antlrcpp::Any SymbolGenVisitor::visitRvalue(ifccParser::RvalueContext* ctx) {
    if (ctx->CONST() != nullptr) {
        memory_offset -= 4;
        tmp_index++;
        variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    }
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitLvalue(ifccParser::LvalueContext* ctx) { return 0; }

int SymbolGenVisitor::check_exist(ifccParser::RvalueContext* ctx) {
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
