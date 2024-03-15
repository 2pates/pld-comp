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
    int tmp_var_address = variables.at(tmp_var_name).address;
    if (s == "*")
        cfg->current_bb->add_IRInstr(IRInstr::Operation::mul, Type::INT32, {a, b, tmp_var_name});
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
    int left_var_size = variables.at(left_var_name).size;
    int left_var_address = variables.at(left_var_name).address;

    std::string right_var_name = visit(ctx->expr()[1]);
    int right_var_size = variables.at(right_var_name).size;
    int right_var_address = variables.at(right_var_name).address;

    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
    int tmp_var_size = variables.at(tmp_var_name).size;
    int tmp_var_address = variables.at(tmp_var_name).address;

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
    int left_var_size = variables.at(left_var_name).size;
    int left_var_address = variables.at(left_var_name).address;

    std::string right_var_name = visit(ctx->expr()[1]);
    int right_var_size = variables.at(right_var_name).size;
    int right_var_address = variables.at(right_var_name).address;

    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
    int tmp_var_size = variables.at(tmp_var_name).size;
    int tmp_var_address = variables.at(tmp_var_name).address;

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

antlrcpp::Any CodeGenVisitor::visitBitwise(std::string l_var_name, char OP, std::string r_var_name) {
    debug("visit bitwise " + OP);
    int l_var_size = variables.at(l_var_name).size;
    int l_var_address = variables.at(l_var_name).address;
    int r_var_size = variables.at(r_var_name).size;
    int r_var_address = variables.at(r_var_name).address;

    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
    int tmp_var_size = variables.at(tmp_var_name).size;
    int tmp_var_address = variables.at(tmp_var_name).address;

    mov(std::to_string(l_var_address) + "(%rbp)", "%eax", 4);
    switch (OP) {
        case '&':
            std::cout << "    andl " << r_var_address << "(%rbp)"
                      << ", %eax\n";
            break;

        case '^':
            std::cout << "    xorl " << r_var_address << "(%rbp)"
                      << ", %eax\n";
            break;

        case '|':
            std::cout << "    orl " << r_var_address << "(%rbp)"
                      << ", %eax\n";
            break;

        default:
            break;
    }

    push_stack("%eax", tmp_var_address, tmp_var_size);
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

antlrcpp::Any CodeGenVisitor::visitExpr_lazy_and(ifccParser::Expr_lazy_andContext* ctx) { return 0; }

antlrcpp::Any CodeGenVisitor::visitExpr_lazy_or(ifccParser::Expr_lazy_orContext* ctx) { return 0; }

antlrcpp::Any CodeGenVisitor::visitExpr_atom(ifccParser::Expr_atomContext* ctx) {
    std::string var_name;
    if (ctx->CONST() != nullptr) {
        tmp_index++;
        var_name = "#tmp" + std::to_string(tmp_index); // we hope that it's the same #tmp number
        if (variables.find(var_name) != variables.end()) {
            cfg->current_bb->add_IRInstr(IRInstr::Operation::ldconst, Type::INT32, {ctx->CONST()->getText(), var_name});
        } else {
            debug("Error: variable " + var_name + " not found");
            return PROGRAMER_ERROR;
        }
    } else if (ctx->VARNAME() != nullptr) {
        var_name = ctx->VARNAME()->getText();
    }
    return var_name;
}
