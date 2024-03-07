#include "SymbolGenVisitor.h"

antlrcpp::Any SymbolGenVisitor::visitProg(ifccParser::ProgContext* ctx) {
    for (auto instr : ctx->instruction()) {
        this->visit(instr);
    }
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitInstruction(ifccParser::InstructionContext* ctx) {
    this->visit(ctx->declare_stmt());
    this->visit(ctx->assignment_stmt());
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext* ctx) { return 0; }

antlrcpp::Any SymbolGenVisitor::visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx) {
    if (ctx->TYPE()->getText() == "int") {
        VariableInfo var(current_index, 4);
        std::string name = ctx->lvalue()->getText();
        variables.insert({name, var});
        current_index -= 4;
    }
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitAssignment_stmt(ifccParser::Assignment_stmtContext* ctx) {
    if (variables.find(ctx->lvalue()->getText()) != variables.end()) {
        return 0;
    }
    return -1; // undeclared variable affectation
}

antlrcpp::Any SymbolGenVisitor::visitRvalue(ifccParser::RvalueContext* ctx) { return 0; }

antlrcpp::Any SymbolGenVisitor::visitLvalue(ifccParser::LvalueContext* ctx) { return 0; }
