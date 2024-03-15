#include "CodeGenVisitor.h"
#include "IR.h"

#ifdef TRACE
#include <iostream>
#define debug(expression) (std::cerr << __FILE__ << ":" << __LINE__ << " -> " << (expression) << std::endl)
#else
#define debug(expression) ((void)0)
#endif

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext* ctx) {
    cfg->add_bb(new BasicBlock(cfg, cfg->entry_block_label));

    for (auto instr : ctx->instruction()) {
        this->visit(instr);
    }

    this->visit(ctx->return_stmt());

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitRvalue(ifccParser::RvalueContext* ctx) {
    debug("r_value");
    return visit(ctx->expr());
}

antlrcpp::Any CodeGenVisitor::visitInstruction(ifccParser::InstructionContext* ctx) {
    if (ctx->assignment_stmt() != nullptr)
        this->visit(ctx->assignment_stmt());
    if (ctx->declare_stmt() != nullptr)
        this->visit(ctx->declare_stmt());
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx) {
    declaration_mode = true;
    visit(ctx->declare());
    declaration_mode = false;
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclare(ifccParser::DeclareContext* ctx) {
    if (ctx->assignment_stmt() != nullptr) {
        visit(ctx->assignment_stmt());
    }
    if (ctx->declare() != nullptr) {
        visit(ctx->declare());
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext* ctx) {
    debug("return_stmt");
    std::string var_name = visit(ctx->expr());
    cfg->current_bb->add_IRInstr(IRInstr::Operation::ret, Type::INT32, {var_name});

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAssignment_stmt(ifccParser::Assignment_stmtContext* ctx) {
    if (declaration_mode || variables.find(ctx->lvalue()->getText()) != cfg->variables.end()) {
        std::string r_name = visit(ctx->rvalue());
        if (variables.find(r_name) != variables.end()) {
            cfg->current_bb->add_IRInstr(IRInstr::Operation::copy, Type::INT32, {r_name, ctx->lvalue()->getText()});
            return 0;
        } else {
            debug("Variable " + r_name + " not found");
            return PROGRAMER_ERROR;
        }
    } else {
        std::cerr << "Undeclared error" << std::endl;
        return UNDECLARED;
    }
}

int CodeGenVisitor::push_stack(int source, int dest) {
    std::cout << "    movq ";
    std::cout << source << "(%rbp)"
              << ", ";
    std::cout << dest << "(%rbp)"
              << "\n";
    return 0;
}

int CodeGenVisitor::push_stack(std::string source, int dest, int size) {
    switch (size) {
        case 8:
            std::cout << "    movq ";
            break;
        case 4:
            std::cout << "    movl ";
            break;
        case 1:
            std::cout << "    movb ";
            break;
        default:
            break;
    }
    std::cout << source << ", ";
    std::cout << dest << "(%rbp)"
              << "\n";
    return 0;
}

int CodeGenVisitor::mov(std::string source, std::string dest, int size) {
    switch (size) {
        case 8:
            std::cout << "    movq ";
            break;
        case 4:
            std::cout << "    movl ";
            break;
        case 1:
            std::cout << "    movb ";
            break;
        default:
            break;
    }
    std::cout << source << ", ";
    std::cout << dest << "\n";
    return 0;
}
