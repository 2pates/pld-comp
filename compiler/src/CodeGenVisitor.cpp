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
    return "0";
}

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

antlrcpp::Any CodeGenVisitor::visitAssignment_equal(ifccParser::Assignment_equalContext* ctx) {
    if (declaration_mode || variables.find(ctx->lvalue()->getText()) != cfg->variables.end()) {
        std::string r_name = visit(ctx->rvalue());
        if (variables.find(r_name) != variables.end()) {
            cfg->current_bb->add_IRInstr(IRInstr::Operation::copy, Type::INT32, {r_name, ctx->lvalue()->getText()});
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

antlrcpp::Any CodeGenVisitor::visitAssignment_add(ifccParser::Assignment_addContext* ctx) {
    std::string s = ctx->OP->getText();
    string a = ctx->lvalue()->getText();
    string b = this->visit(ctx->rvalue());

    if (s == "+=")
        cfg->current_bb->add_IRInstr(IRInstr::Operation::add, Type::INT32, {a, b, a});
    else
        cfg->current_bb->add_IRInstr(IRInstr::Operation::sub, Type::INT32, {a, b, a});
    return a;
}


antlrcpp::Any CodeGenVisitor::visitAssignment_mult(ifccParser::Assignment_multContext* ctx) {
    std::string s = ctx->OP->getText();
    string a = ctx->lvalue()->getText();
    string b = this->visit(ctx->rvalue());

    if (s == "*=")
        cfg->current_bb->add_IRInstr(IRInstr::Operation::mul, Type::INT32, {a, b, a});/*
    else if (s== "/=")
        cfg->current_bb->add_IRInstr(IRInstr::Operation::div, Type::INT32, {a, b, a});
    else
        cfg->current_bb->add_IRInstr(IRInstr::Operation::mod, Type::INT32, {a, b, a});*/
    return a;
}

antlrcpp::Any CodeGenVisitor::visitPre_incrementation(ifccParser::Pre_incrementationContext* ctx) {
    string a = ctx->lvalue()->getText();
    string s = ctx->OP->getText();

    if (s == "++"){
        cfg->current_bb->add_IRInstr(IRInstr::Operation::add_const, Type::INT32, {a, "1", a});
    }
    else{
        cfg->current_bb->add_IRInstr(IRInstr::Operation::sub_const, Type::INT32, {a, "1", a});
    }

    return a;
}

antlrcpp::Any CodeGenVisitor::visitPost_incrementation(ifccParser::Post_incrementationContext* ctx) {
    string a = ctx->lvalue()->getText();
    string s = ctx->OP->getText();
    
    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
    cfg->current_bb->add_IRInstr(IRInstr::Operation::copy, Type::INT32, {a, tmp_var_name});

    if (s == "++"){
        cfg->current_bb->add_IRInstr(IRInstr::Operation::add_const, Type::INT32, {a, "1", a});
    }
    else{
        cfg->current_bb->add_IRInstr(IRInstr::Operation::sub_const, Type::INT32, {a, "1", a});
    }

    return tmp_var_name;
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
