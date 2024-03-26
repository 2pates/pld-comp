#include "CodeGenVisitor.h"
#include "Error.h"
#include "IR.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext* ctx) {
    cfg->add_bb(new BasicBlock(cfg, cfg->entry_block_label));

    for (auto instr : ctx->statement()) {
        this->visit(instr);
    }

    this->visit(ctx->return_stmt());

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitBlock(ifccParser::BlockContext* ctx) {
    tmp_block_index++; // we increase the number of blocks
    current_block = tmp_block_index;
    for (auto instr : ctx->statement()) {
        this->visit(instr);
    }
    current_block = blocks.at(current_block); // the current is now the father
    return GOOD;
}

antlrcpp::Any CodeGenVisitor::visitRvalue(ifccParser::RvalueContext* ctx) { return visit(ctx->expr()); }

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
    std::string lvalue_name = ctx->lvalue()->getText();
    std::string lvalue_unique_name = get_unique_var_name(lvalue_name);
    if (declaration_mode || variables.find(lvalue_unique_name) != cfg->variables.end()) {
        std::string r_name = visit(ctx->rvalue());
        if (variables.find(r_name) != variables.end()) {
            cfg->current_bb->add_IRInstr(IRInstr::Operation::copy, Type::INT32, {r_name, lvalue_unique_name});
            return 0;
        } else {
            debug("Variable " + r_name + " not found");
            return PROGRAMER_ERROR;
        }
    } else {
        error("Error: undeclared variable " + lvalue_unique_name);
        return UNDECLARED;
    }
}

std::string CodeGenVisitor::get_unique_var_name(std::string varname) {
    int block = current_block;
    while (block != -1) { // finds in blocks starting from the upper ones
        std::string unique_var_name = varname + "_" + std::to_string(block);
        if (variables.find(unique_var_name) != variables.end())
            return unique_var_name;
        block = blocks.at(block); // decrease block lvl
    }
    return "";
}
