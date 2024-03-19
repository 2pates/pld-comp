#include "CodeGenVisitor.h"
#include "IR.h"
#include "Error.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext* ctx) {
    cfg->add_bb(new BasicBlock(cfg, cfg->entry_block_label));

    for (auto instr : ctx->instruction()) {
        this->visit(instr);
    }

    this->visit(ctx->return_stmt());

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitRvalue(ifccParser::RvalueContext* ctx) {
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
        error("Error : undeclared error variable" + ctx->lvalue()->getText());
        return UNDECLARED;
    }
}

