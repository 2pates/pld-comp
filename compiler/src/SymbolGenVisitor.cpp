#include "SymbolGenVisitor.h"
#include "Error.h"


antlrcpp::Any SymbolGenVisitor::visitBlock(ifccParser::BlockContext* ctx) {
    tmp_block_index++;                               // we increase the number of blocks
    blocks.insert({tmp_block_index, current_block}); // set father
    current_block = tmp_block_index;
    for (auto instr : ctx->statement()) {
        this->visit(instr);
    }
    current_block = blocks.at(current_block); // the current is now the father
    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitFunction_call(ifccParser::Function_callContext* ctx) {
    //needs to be implemented to say that variables are used
    //assigns memory in case it's being used in an expression

    for(int i=0;i<ctx->expr().size();i++){
        visit(ctx->expr()[i]);
    }
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx) {
    declaration_mode = true;
    if (ctx->type()->getText() == "int") {
        visit(ctx->declare());
    }
    declaration_mode = false;
    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitDeclare(ifccParser::DeclareContext* ctx) {
    if (ctx->lvalue() != nullptr) {
        visit(ctx->lvalue());
        std::string name = ctx->lvalue()->getText();
        std::string unique_name = create_unique_var_name(name);
        if (check_exist_in_current_block(name) != GOOD) {
            memory_offset -= 4; // decrement index first !
            VariableInfo var(memory_offset, 4, false);
            variables.insert({unique_name, var});

            debug("Declaration: " + unique_name + " (address " + std::to_string(var.address) + ")");
        } else {
            error("Error: already used name" + unique_name);
            exit(DOUBLE_DECLARATION);
        }
    } else if (ctx->assignment_stmt() != nullptr) {
        visit(ctx->assignment_stmt());
    }
    if (ctx->declare() != nullptr) {
        visit(ctx->declare());
    }
    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitAssignment_stmt(ifccParser::Assignment_stmtContext* ctx) {
    std::string name = ctx->lvalue()->VARNAME()->getText();
    std::string unique_name = create_unique_var_name(name);
    if (declaration_mode) {
        if (check_exist_in_current_block(name) == GOOD) {
            exit(DOUBLE_DECLARATION);
        } else {
            visit(ctx->lvalue());
            memory_offset -= 4; // decrement index first !
            VariableInfo var(memory_offset, 4, false);
            variables.insert({unique_name, var});

            visit(ctx->rvalue());
        }
        return GOOD;
    } else if (check_exist_in_current_or_parent_block(name) == GOOD) {
        visit(ctx->rvalue());
        debug("Affectation: " + unique_name + " = " + ctx->rvalue()->getText());
        return GOOD;
    } else {
        error("Error: undeclared variable " + unique_name);
        exit(UNDECLARED); // undeclared variable affectation
    }
}

antlrcpp::Any SymbolGenVisitor::visitSelection_if(ifccParser::Selection_ifContext* ctx) {
    this->visit(ctx->expr());
    this->visit(ctx->instruction()[0]);
    if(ctx->instruction()[1] != nullptr) {
        this->visit(ctx->instruction()[1]);
    }
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitIteration_while(ifccParser::Iteration_whileContext* ctx) {
    this->visit(ctx->expr());
    this->visit(ctx->instruction());
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitLvalue(ifccParser::LvalueContext* ctx) {
    std::string name = ctx->VARNAME()->getText();
    if (std::find(reserved_word.begin(), reserved_word.end(), name) != reserved_word.end()) {
        error("Error: reserved keyword " + name);
        exit(RESERVED_KEY_WORD);
    }
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_relational(ifccParser::Expr_relationalContext* ctx) {
    visit(ctx->expr()[0]);
    visit(ctx->expr()[1]);

    memory_offset -= 4;
    variables.insert({get_new_tmp_varname(), VariableInfo(memory_offset, 4)});
    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_equality(ifccParser::Expr_equalityContext* ctx) {
    visit(ctx->expr()[0]);
    visit(ctx->expr()[1]);

    memory_offset -= 4;
    variables.insert({get_new_tmp_varname(), VariableInfo(memory_offset, 4)});
    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_atom(ifccParser::Expr_atomContext* ctx) {
    if (ctx->CONST() != nullptr) {
        memory_offset -= 4;
        variables.insert({get_new_tmp_varname(), VariableInfo(memory_offset, 4)});
    }
    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_add(ifccParser::Expr_addContext* ctx) {
    visit(ctx->expr()[0]);
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    debug("Inserted (Expr_add) #tmp" + std::to_string(tmp_index) + " (address " + std::to_string(memory_offset) + ")");
    visit(ctx->expr()[1]);
    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_and(ifccParser::Expr_andContext* ctx) {
    visit(ctx->expr()[0]);
    visit(ctx->expr()[1]);
    memory_offset -= 4;
    variables.insert({get_new_tmp_varname(), VariableInfo(memory_offset, 4)});
    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_xor(ifccParser::Expr_xorContext* ctx) {
    visit(ctx->expr()[0]);
    visit(ctx->expr()[1]);
    memory_offset -= 4;
    variables.insert({get_new_tmp_varname(), VariableInfo(memory_offset, 4)});
    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_or(ifccParser::Expr_orContext* ctx) {
    visit(ctx->expr()[0]);
    visit(ctx->expr()[1]);
    memory_offset -= 4;
    variables.insert({get_new_tmp_varname(), VariableInfo(memory_offset, 4)});
    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_lazy_and(ifccParser::Expr_lazy_andContext* ctx) {
    visit(ctx->expr()[0]);
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    visit(ctx->expr()[1]);
    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_lazy_or(ifccParser::Expr_lazy_orContext* ctx) {
    visit(ctx->expr()[0]);
    visit(ctx->expr()[1]);
    memory_offset -= 4;
    variables.insert({get_new_tmp_varname(), VariableInfo(memory_offset, 4)});
    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_unaire(ifccParser::Expr_unaireContext* ctx) {
    visit(ctx->expr());
    memory_offset -= 4;
    variables.insert({get_new_tmp_varname(), VariableInfo(memory_offset, 4)});
    return GOOD;
}

int SymbolGenVisitor::check_exist_in_current_block(std::string varname) {
    if (variables.find(create_unique_var_name(varname)) != variables.end())
        return GOOD;
    else
        return UNDECLARED;
}

int SymbolGenVisitor::check_exist_in_current_or_parent_block(std::string varname) {
    int block = current_block;
    while (block != -1) {
        std::string unique_var_name = varname + "_" + std::to_string(block);
        if (variables.find(unique_var_name) != variables.end())
            return GOOD;
        block = blocks.at(block);
    }
    return UNDECLARED;
}

std::string SymbolGenVisitor::get_new_tmp_varname() {
    tmp_index++;
    return "#tmp" + std::to_string(tmp_index);
}

std::string SymbolGenVisitor::create_unique_var_name(std::string name) {
    return name + "_" + std::to_string(current_block);
}

antlrcpp::Any SymbolGenVisitor::visitExpr_mult(ifccParser::Expr_multContext* ctx){
    this->visit(ctx->expr(0));
    this->visit(ctx->expr(1));
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_parenthesis(ifccParser::Expr_parenthesisContext* ctx) {
    visit(ctx->expr());
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    return 0;
}

