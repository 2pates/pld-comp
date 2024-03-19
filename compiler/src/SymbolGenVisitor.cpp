#include "SymbolGenVisitor.h"
#include "Error.h"

    antlrcpp::Any SymbolGenVisitor::visitAxiom(ifccParser::AxiomContext *ctx)
    {
        return visit(ctx->prog());
    }


antlrcpp::Any SymbolGenVisitor::visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx) {
    declaration_mode = true;
    if (ctx->TYPE()->getText() == "int") {
        visit(ctx->declare());
    }
    declaration_mode = false;
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitDeclare(ifccParser::DeclareContext* ctx) {
    if (ctx->lvalue() != nullptr) {
        std::string name = ctx->lvalue()->getText();
        if (check_exist(name) != 0) {
            memory_offset -= 4; // decrement index first !
            VariableInfo var(memory_offset, 4, false);
            variables.insert({name, var});

            debug("Declaration: " + name + " (address " +  std::to_string(var.address)  + ")");
        } else {
            error("Error: already used name" + name);
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
    if (variables.find(ctx->lvalue()->getText()) != variables.end()) {
        visit(ctx->rvalue());
        debug("Affectation: " +  ctx->lvalue()->getText() + " = " + ctx->rvalue()->getText());
        return 0;
    } else if (declaration_mode) {
        visit(ctx->lvalue());
        visit(ctx->rvalue());
        return 0;
    } else {
        error("Error: undeclared variable " + ctx->lvalue()->getText());
        return UNDECLARED; // undeclared variable affectation
    }
}

antlrcpp::Any SymbolGenVisitor::visitLvalue(ifccParser::LvalueContext* ctx) {
    std::string name = ctx->VARNAME()->getText();
    if (std::find(reserved_word.begin(), reserved_word.end(), name) != reserved_word.end()) {
        error("Error: reserved keyword " + name);
        return RESERVED_KEY_WORD;
    } else {
        memory_offset -= 4; // decrement index first !
        VariableInfo var(memory_offset, 4, false);
        variables.insert({name, var});
        return 0;
    }
}

antlrcpp::Any SymbolGenVisitor::visitExpr_relational(ifccParser::Expr_relationalContext* ctx) {
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_equality(ifccParser::Expr_equalityContext* ctx) {
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    return 0;
}


antlrcpp::Any SymbolGenVisitor::visitExpr_atom(ifccParser::Expr_atomContext* ctx) {
    if (ctx->CONST() != nullptr) {
        memory_offset -= 4;
        tmp_index++;
        variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    }
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_and(ifccParser::Expr_andContext* ctx) {
    visit(ctx->expr()[0]);
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    visit(ctx->expr()[1]);
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_xor(ifccParser::Expr_xorContext* ctx) {
    visit(ctx->expr()[0]);
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    visit(ctx->expr()[1]);
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_or(ifccParser::Expr_orContext* ctx) {
    visit(ctx->expr()[0]);
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    visit(ctx->expr()[1]);
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_unaire(ifccParser::Expr_unaireContext* ctx) {
    visit(ctx->expr());
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
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
antlrcpp::Any SymbolGenVisitor::visitExpr_add(ifccParser::Expr_addContext* ctx){
    this->visit(ctx->expr(0));
    this->visit(ctx->expr(1));
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    return 0;
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