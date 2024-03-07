#include "SymbolGenVisitor.h"

antlrcpp::Any SymbolGenVisitor::visitProg(ifccParser::ProgContext* ctx) {
    for (auto instr : ctx->instruction()) {
        this->visit(instr);
    }
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitInstruction(ifccParser::InstructionContext* ctx) {
    if (ctx->declare_stmt() != nullptr) {
        this->visit(ctx->declare_stmt());
    }
    if (ctx->assignment_stmt() != nullptr) {
        this->visit(ctx->assignment_stmt());
    }
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext* ctx) { return 0; }

antlrcpp::Any SymbolGenVisitor::visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx) {
    if (ctx->TYPE()->getText() == "int") {
        VariableInfo var(current_index, 4);
        std::string name = ctx->lvalue()->getText();
        variables.insert({name, var});
        current_index -= 4;
        std::cout << "Declaration: " << ctx->TYPE()->getText() << " " << name << " (address " << var.address << ")" << std::endl;
    }
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitAssignment_stmt(ifccParser::Assignment_stmtContext* ctx) {
    if (variables.find(ctx->lvalue()->getText()) != variables.end()) {
        std::cout << "Affectation: " << ctx->lvalue()->getText() << " = " << ctx->rvalue()->getText() << std::endl;
        return 0;
    }
    std::cerr << "Error: undeclared variable " << ctx->lvalue()->getText() << std::endl;
    return -1; // undeclared variable affectation
}

antlrcpp::Any SymbolGenVisitor::visitRvalue(ifccParser::RvalueContext* ctx) { return 0; }

antlrcpp::Any SymbolGenVisitor::visitLvalue(ifccParser::LvalueContext* ctx) { return 0; }
