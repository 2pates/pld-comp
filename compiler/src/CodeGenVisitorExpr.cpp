#include "CodeGenVisitor.h"

#ifdef TRACE
#include <iostream>
#define debug(expression) (std::cerr << __FILE__ << ":" << __LINE__ << " -> " << (expression) << std::endl)
#else
#define debug(expression) ((void)0)
#endif

antlrcpp::Any CodeGenVisitor::visitExpr_parenthesis(ifccParser::Expr_parenthesisContext* ctx) {
    std::string var_name = visit(ctx->expr());
    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
    cfg->current_bb->add_IRInstr(IRInstr::Operation::copy, Type::INT32, {var_name, tmp_var_name});
    return tmp_var_name;
}
antlrcpp::Any CodeGenVisitor::visitExpr_function(ifccParser::Expr_functionContext* ctx) {
    std::string var_name = visit(ctx->function_call());
    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
    cfg->current_bb->add_IRInstr(IRInstr::Operation::copy, Type::INT32, {var_name, tmp_var_name});
    return tmp_var_name;
}


antlrcpp::Any CodeGenVisitor::visitExpr_unaire(ifccParser::Expr_unaireContext* ctx) {
    std::string var_name = visit(ctx->expr());
    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
    switch (ctx->OP->getText()[0]) {
        case '~':
            cfg->current_bb->add_IRInstr(IRInstr::Operation::bitwise_not, Type::INT32, {var_name, tmp_var_name});
            break;
        case '-':
            cfg->current_bb->add_IRInstr(IRInstr::Operation::neg, Type::INT32, {var_name, tmp_var_name});
            break;
        case '!':
            cfg->current_bb->add_IRInstr(IRInstr::Operation::l_not, Type::INT32, {var_name, tmp_var_name});
            break;
        default:
            break;
    }
    return tmp_var_name;
}

antlrcpp::Any CodeGenVisitor::visitExpr_mult(ifccParser::Expr_multContext* ctx) {
    std::string s = ctx->OP->getText();
    string a = this->visit(ctx->expr(0));
    string b = this->visit(ctx->expr(1));
    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
    if (s == "*") {
        cfg->current_bb->add_IRInstr(IRInstr::Operation::mul, Type::INT32, {a, b, tmp_var_name});
    } else if (s == "/") {
        cfg->current_bb->add_IRInstr(IRInstr::Operation::div, Type::INT32, {a, b, tmp_var_name});
    } else if (s == "%") {
        cfg->current_bb->add_IRInstr(IRInstr::Operation::mod, Type::INT32, {a, b, tmp_var_name});
    }
    return tmp_var_name;
}

antlrcpp::Any CodeGenVisitor::visitExpr_add(ifccParser::Expr_addContext* ctx) {
    std::string s = ctx->OP->getText();
    string a = this->visit(ctx->expr(0));
    string b = this->visit(ctx->expr(1));
    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
    if (s == "+")
        cfg->current_bb->add_IRInstr(IRInstr::Operation::add, Type::INT32, {a, b, tmp_var_name});
    else
        cfg->current_bb->add_IRInstr(IRInstr::Operation::sub, Type::INT32, {a, b, tmp_var_name});
    return tmp_var_name;
}

antlrcpp::Any CodeGenVisitor::visitExpr_relational(ifccParser::Expr_relationalContext* ctx) {
    std::string left_var_name = visit(ctx->expr()[0]);

    std::string right_var_name = visit(ctx->expr()[1]);

    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);

    std::string ope = ctx->OP->getText();
    if (ope == "<") {
        cfg->current_bb->add_IRInstr(IRInstr::Operation::cmp_lt, Type::INT32,
                                     {left_var_name, right_var_name, tmp_var_name});
    } else if (ope == "<=") {
        cfg->current_bb->add_IRInstr(IRInstr::Operation::cmp_le, Type::INT32,
                                     {left_var_name, right_var_name, tmp_var_name});
    } else if (ope == ">") {
        cfg->current_bb->add_IRInstr(IRInstr::Operation::cmp_gt, Type::INT32,
                                     {left_var_name, right_var_name, tmp_var_name});
    } else if (ope == ">=") {
        cfg->current_bb->add_IRInstr(IRInstr::Operation::cmp_ge, Type::INT32,
                                     {left_var_name, right_var_name, tmp_var_name});
    }
    return tmp_var_name;
}

antlrcpp::Any CodeGenVisitor::visitExpr_equality(ifccParser::Expr_equalityContext* ctx) {
    std::string left_var_name = visit(ctx->expr()[0]);

    std::string right_var_name = visit(ctx->expr()[1]);

    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);

    std::string ope = ctx->OP->getText();
    if (ope == "==") {
        cfg->current_bb->add_IRInstr(IRInstr::Operation::cmp_eq, Type::INT32,
                                     {left_var_name, right_var_name, tmp_var_name});
    } else if (ope == "!=") {
        cfg->current_bb->add_IRInstr(IRInstr::Operation::cmp_ne, Type::INT32,
                                     {left_var_name, right_var_name, tmp_var_name});
    }
    return tmp_var_name;
}

antlrcpp::Any CodeGenVisitor::visitExpr_and(ifccParser::Expr_andContext* ctx) {
    std::string l_var_name = visit(ctx->expr().at(0));
    std::string r_var_name = visit(ctx->expr().at(1));
    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
    cfg->current_bb->add_IRInstr(IRInstr::Operation::bitwise_and, Type::INT32, {l_var_name, r_var_name, tmp_var_name});
    return tmp_var_name;
}

antlrcpp::Any CodeGenVisitor::visitExpr_xor(ifccParser::Expr_xorContext* ctx) {
    std::string l_var_name = visit(ctx->expr().at(0));
    std::string r_var_name = visit(ctx->expr().at(1));
    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
    cfg->current_bb->add_IRInstr(IRInstr::Operation::bitwise_xor, Type::INT32, {l_var_name, r_var_name, tmp_var_name});
    return tmp_var_name;
}

antlrcpp::Any CodeGenVisitor::visitExpr_or(ifccParser::Expr_orContext* ctx) {
    std::string l_var_name = visit(ctx->expr().at(0));
    std::string r_var_name = visit(ctx->expr().at(1));
    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
    cfg->current_bb->add_IRInstr(IRInstr::Operation::bitwise_or, Type::INT32, {l_var_name, r_var_name, tmp_var_name});
    return tmp_var_name;
}

antlrcpp::Any CodeGenVisitor::visitExpr_lazy_and(ifccParser::Expr_lazy_andContext* ctx) {
    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);

    std::string l_var_name = visit(ctx->expr().at(0));
    std::string r_var_name = visit(ctx->expr().at(1));

    BasicBlock* nextBB = new BasicBlock(cfg, cfg->new_BB_name());
    cfg->add_bb(nextBB);

    BasicBlock* rightBB = new BasicBlock(cfg, cfg->new_BB_name());
    cfg->add_bb(rightBB);
    rightBB->add_IRInstr(IRInstr::Operation::cmp_const, Type::INT32, {r_var_name, "0", tmp_var_name}); // IF right == false
    rightBB->add_IRInstr(IRInstr::Operation::l_not, Type::INT32, {tmp_var_name, tmp_var_name}); // NOT (right == false)
    rightBB->exit_true = nextBB;

    BasicBlock* leftBB = new BasicBlock(cfg, cfg->new_BB_name());
    cfg->add_bb(leftBB);
    leftBB->add_IRInstr(IRInstr::Operation::cmp_const, Type::INT32, {l_var_name, "0", tmp_var_name}); // IF left == false
    leftBB->add_IRInstr(IRInstr::Operation::l_not, Type::INT32, {tmp_var_name, tmp_var_name}); // NOT (left == false)
    leftBB->test_var_name = tmp_var_name;
    leftBB->exit_true = rightBB; // IF left == true then jump to right -> check if right is also true
    leftBB->exit_false = nextBB; // IF left == false then jump to next -> result = false

    cfg->current_bb->exit_true = leftBB;
    cfg->current_bb = nextBB;

    return tmp_var_name;
}

antlrcpp::Any CodeGenVisitor::visitExpr_lazy_or(ifccParser::Expr_lazy_orContext* ctx) {
    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);

    std::string l_var_name = visit(ctx->expr().at(0));
    std::string r_var_name = visit(ctx->expr().at(1));

    BasicBlock* nextBB = new BasicBlock(cfg, cfg->new_BB_name());
    cfg->add_bb(nextBB);

    BasicBlock* rightBB = new BasicBlock(cfg, cfg->new_BB_name());
    cfg->add_bb(rightBB);
    rightBB->add_IRInstr(IRInstr::Operation::cmp_const, Type::INT32, {r_var_name, "0", tmp_var_name}); // IF right == false
    rightBB->add_IRInstr(IRInstr::Operation::l_not, Type::INT32, {tmp_var_name, tmp_var_name}); // NOT (right == false)
    rightBB->exit_true = nextBB;

    BasicBlock* leftBB = new BasicBlock(cfg, cfg->new_BB_name());
    cfg->add_bb(leftBB);
    leftBB->add_IRInstr(IRInstr::Operation::cmp_const, Type::INT32, {l_var_name, "0", tmp_var_name}); // IF left == false
    leftBB->add_IRInstr(IRInstr::Operation::l_not, Type::INT32, {tmp_var_name, tmp_var_name}); // NOT (left == false)
    leftBB->test_var_name = tmp_var_name;
    leftBB->exit_true = nextBB; // IF left == true then jump to next -> result = true
    leftBB->exit_false = rightBB; // IF left == false then jump to right -> result = check if right is true

    cfg->current_bb->exit_true = leftBB;
    cfg->current_bb = nextBB;
    return tmp_var_name;
}

antlrcpp::Any CodeGenVisitor::visitExpr_atom(ifccParser::Expr_atomContext* ctx) {
    std::string var_name;
    if (ctx->CONST_INT() != nullptr) {
        tmp_index++;
        var_name = "#tmp" + std::to_string(tmp_index); // we hope that it's the same #tmp number
        debug(var_name);
        if (variables.find(var_name) != variables.end()) {
            cfg->current_bb->add_IRInstr(IRInstr::Operation::ldconst, Type::INT32, {ctx->CONST_INT()->getText(), var_name});
        } else {
            debug("Error: variable " + var_name + " not found");
            return PROGRAMER_ERROR;
        }
    } else if (ctx->CONST_CHAR() != nullptr) {
        tmp_index++;
        var_name = "#tmp" + std::to_string(tmp_index); // we hope that it's the same #tmp number
        debug(var_name);
        if (variables.find(var_name) != variables.end()) {
            int numerical_value = int(ctx->CONST_CHAR()->getText()[1]);
			std::string value = std::to_string(numerical_value);
            debug(value);
            cfg->current_bb->add_IRInstr(IRInstr::Operation::ldconst, Type::INT32, {value, var_name});
        } else {
            debug("Error: variable " + var_name + " not found");
            return PROGRAMER_ERROR;
        }
    } else if (ctx->CONST_VOID() != nullptr) {
        tmp_index++;
        var_name = "#tmp" + std::to_string(tmp_index); // we hope that it's the same #tmp number
        debug(var_name);
        if (variables.find(var_name) != variables.end()) {
            if (ctx->CONST_VOID()->getText() == "NULL") {
                cfg->current_bb->add_IRInstr(IRInstr::Operation::ldconst, Type::POINTER, {"0", var_name});
            }
        } else {
            debug("Error: variable " + var_name + " not found");
            return PROGRAMER_ERROR;
        }
    } else if (ctx->VARNAME() != nullptr) {
        var_name = get_unique_var_name(ctx->VARNAME()->getText());
        debug(var_name);
    }
    return var_name;
}

antlrcpp::Any CodeGenVisitor::visitExpr_assignment(ifccParser::Expr_assignmentContext* ctx) {
    string var_name = visit(ctx->assignment_stmt());
    return var_name;
}

