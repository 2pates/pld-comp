#include "CodeGenVisitor.h"
#include "Error.h"
#include "IR.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext* ctx) {
    BasicBlock* bb = new BasicBlock(cfg, cfg->entry_block_label);
    cfg->current_bb = bb;
    cfg->add_bb(bb);

    for (auto instr : ctx->statement()) {
        this->visit(instr);
    }
    this->visit(ctx->return_stmt());
    for (auto function : ctx->function_def()) {
        this->visit(function);
    }    
    return 0;
}
antlrcpp::Any CodeGenVisitor::visitFunction_def(ifccParser::Function_defContext* ctx) {
    BasicBlock* bb = new BasicBlock(cfg, "function_"+ctx->VARNAME()->getText());
    cfg->current_bb = bb;
    cfg->add_bb(bb);
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

antlrcpp::Any CodeGenVisitor::visitFunction_call(ifccParser::Function_callContext *ctx){
    std::string s=ctx->VARNAME()->getText();
    if(s=="putchar" && ctx->expr().size()==1 && ctx->expr()[0]!=nullptr){
        std::string var_name = visit(ctx->expr()[0]);
        cfg->current_bb->add_IRInstr(IRInstr::Operation::copyIn, Type::INT32, {var_name, "%edi"});
        cfg->current_bb->add_IRInstr(IRInstr::Operation::call, Type::INT32, {"putchar@PLT"});
        std::string tmp_var_name_return = "#tmp" + std::to_string(tmp_index);
        cfg->current_bb->add_IRInstr(IRInstr::Operation::copyOut, Type::INT32, {"%eax",tmp_var_name_return});
        return tmp_var_name_return;
    }    
    if(s=="getchar" && ctx->expr().size()==0){
        cfg->current_bb->add_IRInstr(IRInstr::Operation::call, Type::INT32, {"getchar@PLT"});
        std::string tmp_var_name_return = "#tmp" + std::to_string(tmp_index);
        cfg->current_bb->add_IRInstr(IRInstr::Operation::copyOut, Type::INT32, {"%eax",tmp_var_name_return});
        return tmp_var_name_return;        
    }
    cfg->current_bb->add_IRInstr(IRInstr::Operation::jump, Type::INT32, {"function_"+s});
    std::string tmp_var_name_return = "#tmp" + std::to_string(tmp_index);
    cfg->current_bb->add_IRInstr(IRInstr::Operation::copyOut, Type::INT32, {"%eax",tmp_var_name_return});
    return tmp_var_name_return;}

antlrcpp::Any CodeGenVisitor::visitRvalue(ifccParser::RvalueContext* ctx) { return visit(ctx->expr()); }

antlrcpp::Any CodeGenVisitor::visitInstruction(ifccParser::InstructionContext* ctx) {
    if (ctx->assignment_stmt() != nullptr)
        this->visit(ctx->assignment_stmt());
    if (ctx->declare_stmt() != nullptr)
        this->visit(ctx->declare_stmt());
    if (ctx->function_call() != nullptr) {
        this->visit(ctx->function_call());
    }
    if (ctx->selection_stmt() != nullptr)
        this->visit(ctx->selection_stmt());
    if (ctx->iterationStatement() != nullptr)
        this->visit(ctx->iterationStatement());
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
            return r_name;
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

antlrcpp::Any CodeGenVisitor::visitSelection_if(ifccParser::Selection_ifContext* ctx) {
    BasicBlock* nextBB = new BasicBlock(cfg, cfg->new_BB_name());
    cfg->add_bb(nextBB);
    BasicBlock* testBlock = new BasicBlock(cfg, cfg->new_BB_name());
    cfg->add_bb(testBlock);
    
    string testVarName = visit(ctx->expr());
    testBlock->test_var_name = testVarName;

    cfg->current_bb->exit_true = testBlock; // After current block, jump to testBlock
    
    BasicBlock* thenBB = new BasicBlock(cfg, cfg->new_BB_name());
    cfg->add_bb(thenBB);
    //After then block, jump to nextBB, might be overwritten during visit(ctx->instruction(0))
    thenBB->exit_true = nextBB;
    cfg->current_bb = thenBB;
    visit(ctx->statement()[0]);

    //If test is true jump to thenBB
    testBlock->exit_true = thenBB;
    //If test is false jump to nextBB
    testBlock->exit_false = nextBB;

    if (ctx->statement()[1] != nullptr) {
        //If else statement
        BasicBlock* elseBB = new BasicBlock(cfg, cfg->new_BB_name());
        cfg->add_bb(elseBB);
        //After else block, jump to nextBB, might be overwritten during visit(ctx->instruction(1))
        elseBB->exit_true = nextBB;

        cfg->current_bb = elseBB;
        visit(ctx->statement(1));

        // If test is false, jump to elseBB
        testBlock->exit_false = elseBB;
        
    }

    cfg->current_bb = nextBB;
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitIteration_while(ifccParser::Iteration_whileContext* ctx) {
    BasicBlock* nextBB = new BasicBlock(cfg, cfg->new_BB_name());
    cfg->add_bb(nextBB);
    BasicBlock* testBlock = new BasicBlock(cfg, cfg->new_BB_name());
    cfg->add_bb(testBlock);

    cfg->current_bb->exit_true = testBlock; // After current block, jump to testBlock
    
    BasicBlock* thenBB = new BasicBlock(cfg, cfg->new_BB_name());
    cfg->add_bb(thenBB);
    //After then block, jump to nextBB, might be overwritten during visit(ctx->instruction(0))
    thenBB->exit_true = testBlock;
    cfg->current_bb = thenBB;
    visit(ctx->statement());

    //If test is true jump to thenBB
    testBlock->exit_true = thenBB;
    //If test is false jump to nextBB
    testBlock->exit_false = nextBB;

    cfg->current_bb = testBlock;
    string testVarName = visit(ctx->expr());
    testBlock->test_var_name = testVarName;


    cfg->current_bb = nextBB;
    return 0;
}
