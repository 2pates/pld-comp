#include "SymbolGenVisitor.h"
#include "Error.h"

antlrcpp::Any SymbolGenVisitor::visitProg(ifccParser::ProgContext* ctx) {
    currentFunction = "main";
    visit(ctx->block());

    for (auto function : ctx->function_def()) {
        visit(function);
    }
    return GOOD;
}

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
    // needs to be implemented to say that variables are used
    // assigns memory in case it's being used in an expression

    for (long unsigned int i = 0; i < ctx->expr().size(); i++) {
        visit(ctx->expr()[i]);
    }
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitFunction_def(ifccParser::Function_defContext* ctx) {
    currentFunction = ctx->VARNAME()->getText(); // TODO: check if reserved keyword
    return visitChildren(ctx);
}

antlrcpp::Any SymbolGenVisitor::visitDeclare_only_stmt(ifccParser::Declare_only_stmtContext* ctx) {
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
    if (ctx->declare_only_stmt() != nullptr) {
        visit(ctx->declare_only_stmt());
    }
    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx) {
    declaration_mode = true;
    std::string name;
    std::string unique_name;
    VariableInfo var;

    for (auto it : ctx->lvalue()) {
        visit(it);
        name = it->getText();
        unique_name = create_unique_var_name(name);
        if (check_exist_in_current_block(name) != GOOD) {
            if (it->pointer_type() != nullptr) {
                memory_offset -= 8; // decrement index first !
                var = VariableInfo(memory_offset, 8, false);
            } else if (ctx->type()->getText() == "int") {
                memory_offset -= 4; // decrement index first !
                var = VariableInfo(memory_offset, 4, false);
            } else if (ctx->type()->getText() == "char") {
                memory_offset -= 4; // decrement index first !
                var = VariableInfo(memory_offset, 4, false);
            } else if (ctx->type()->getText() == "void") {
                exit(VOID_VARIABLE);
            }
            variables.insert({unique_name, var});

            debug("Declaration: " + unique_name + " (address " + std::to_string(var.address) + ")");
        } else {
            error("Error: already used name" + unique_name);
            exit(DOUBLE_DECLARATION);
        }
    }
    for (auto it : ctx->assignment_stmt()) {
        if (ifccParser::Assignment_equalContext* child_ctx = dynamic_cast<ifccParser::Assignment_equalContext*>(it)) {
            name = child_ctx->lvalue()->VARNAME()->getText();
            unique_name = create_unique_var_name(name);
            if (check_exist_in_current_block(name) == GOOD) {
                exit(DOUBLE_DECLARATION);
            } else {
                visit(child_ctx->lvalue());
                if (child_ctx->lvalue()->pointer_type() != nullptr) {
                    memory_offset -= 8; // decrement index first !
                    var = VariableInfo(memory_offset, 8, false);
                } else if (ctx->type()->getText() == "int") {
                    memory_offset -= 4; // decrement index first !
                    var = VariableInfo(memory_offset, 4, false);
                } else if (ctx->type()->getText() == "char") {
                    memory_offset -= 1; // decrement index first !
                    var = VariableInfo(memory_offset, 4, false);
                    memory_offset -= 3;
                } else if (ctx->type()->getText() == "void") {
                    exit(VOID_VARIABLE);
                }
                variables.insert({unique_name, var});

                visit(child_ctx->rvalue());
            }
        } else {
            visit(it);
        }
    }
    declaration_mode = false;
    return GOOD;
}
// antlrcpp::Any SymbolGenVisitor::visitDeclare(ifccParser::DeclareContext* ctx) {
//      if (ctx->lvalue() != nullptr) {
//          visit(ctx->lvalue());
//          std::string name = ctx->lvalue()->getText();
//          std::string unique_name = create_unique_var_name(name);
//          if (check_exist_in_current_block(name) != GOOD) {
//              if (it->pointer_type() != nullptr) {
//                  memory_offset -= 8; // decrement index first !
//                  var = VariableInfo(memory_offset, 8, false);
//              } else if (ctx->type()->getText() == "int") {
//                  memory_offset -= 4; // decrement index first !
//                  var = VariableInfo(memory_offset, 4, false);
//              } else if (ctx->type()->getText() == "char") {
//                  memory_offset -= 4; // decrement index first !
//                  var = VariableInfo(memory_offset, 4, false);
//              } else if (ctx->type()->getText() == "void") {
//                  exit(VOID_VARIABLE);
//              }
//              variables.insert({unique_name, var});

//             debug("Declaration: " + unique_name + " (address " + std::to_string(var.address) + ")");
//         } else {
//             error("Error: already used name" + unique_name);
//             exit(DOUBLE_DECLARATION);
//         }
//     }
//     for (auto it : ctx->assignment_stmt()) {
//         name = it->lvalue()->VARNAME()->getText();
//         unique_name = create_unique_var_name(name);
//         if (check_exist_in_current_block(name) == GOOD) {
//             exit(DOUBLE_DECLARATION);
//         } else {
//             visit(it->lvalue());
//             if (it->lvalue()->pointer_type() != nullptr) {
//                 memory_offset -= 8; // decrement index first !
//                 var = VariableInfo(memory_offset, 8, false);
//             } else if (ctx->type()->getText() == "int") {
//                 memory_offset -= 4; // decrement index first !
//                 var = VariableInfo(memory_offset, 4, false);
//             } else if (ctx->type()->getText() == "char") {
//                 memory_offset -= 1; // decrement index first !
//                 var = VariableInfo(memory_offset, 4, false);
//                 memory_offset -= 3;
//             } else if (ctx->type()->getText() == "void") {
//                 exit(VOID_VARIABLE);
//             }
//             variables.insert({unique_name, var});

//             visit(it->rvalue());
//         }
//     }
//     declaration_mode = false;
//     return GOOD;
// }

// antlrcpp::Any SymbolGenVisitor::visitDeclare(ifccParser::DeclareContext* ctx) {
//     if (ctx->lvalue() != nullptr) {
//         visit(ctx->lvalue());
//         std::string name = ctx->lvalue()->getText();
//         std::string unique_name = create_unique_var_name(name);
//         if (check_exist_in_current_block(name) != GOOD) {
//             if (it->pointer_type() != nullptr) {
//                 memory_offset -= 8; // decrement index first !
//                 var = VariableInfo(memory_offset, 8, false);
//             } else if (ctx->type()->getText() == "int") {
//                 memory_offset -= 4; // decrement index first !
//                 var = VariableInfo(memory_offset, 4, false);
//             } else if (ctx->type()->getText() == "char") {
//                 memory_offset -= 4; // decrement index first !
//                 var = VariableInfo(memory_offset, 4, false);
//             } else if (ctx->type()->getText() == "void") {
//                 exit(VOID_VARIABLE);
//             }
//             variables.insert({unique_name, var});

//             debug("Declaration: " + unique_name + " (address " + std::to_string(var.address) + ")");
//         } else {
//             error("Error: already used name" + unique_name);
//             exit(DOUBLE_DECLARATION);
//         }
//     }
//     for (auto it : ctx->assignment_stmt()) {
//         name = it->lvalue()->VARNAME()->getText();
//         unique_name = create_unique_var_name(name);
//         if (check_exist_in_current_block(name) == GOOD) {
//             exit(DOUBLE_DECLARATION);
//         } else {
//             visit(it->lvalue());
//             if (it->lvalue()->pointer_type() != nullptr) {
//                 memory_offset -= 8; // decrement index first !
//                 var = VariableInfo(memory_offset, 8, false);
//             } else if (ctx->type()->getText() == "int") {
//                 memory_offset -= 4; // decrement index first !
//                 var = VariableInfo(memory_offset, 4, false);
//             } else if (ctx->type()->getText() == "char") {
//                 memory_offset -= 1; // decrement index first !
//                 var = VariableInfo(memory_offset, 4, false);
//                 memory_offset -= 3;
//             } else if (ctx->type()->getText() == "void") {
//                 exit(VOID_VARIABLE);
//             }
//             variables.insert({unique_name, var});

//             visit(it->rvalue());
//         }
//     }
//     declaration_mode = false;
//     return GOOD;
// }

antlrcpp::Any SymbolGenVisitor::visitAssignment_equal(ifccParser::Assignment_equalContext* ctx) {
    std::string name = ctx->lvalue()->VARNAME()->getText();
    std::string unique_name = create_unique_var_name(name);
    if(inExpr){
        variables.at(get_unique_var_name(ctx->lvalue()->VARNAME()->getText())).used=true;
    }
    debug("Affectation " + unique_name);
    if (declaration_mode) {
        if (check_exist_in_current_block(name) == GOOD) {
            error("Error: double declared variable " + name);
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
    this->visit(ctx->statement()[0]);
    if (ctx->statement().size()>=2 && ctx->statement()[1] != nullptr) {
        this->visit(ctx->statement()[1]);
    }
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitIteration_while(ifccParser::Iteration_whileContext* ctx) {
    this->visit(ctx->expr());
    this->visit(ctx->statement());
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

antlrcpp::Any SymbolGenVisitor::visitExpr_mult(ifccParser::Expr_multContext* ctx) {

    this->visit(ctx->expr(0));
    this->visit(ctx->expr(1));
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    return 0;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_atom(ifccParser::Expr_atomContext* ctx) {
    if (ctx->CONST_INT() != nullptr) {
        memory_offset -= 4;
        variables.insert({get_new_tmp_varname(), VariableInfo(memory_offset, 4)});
    } else if (ctx->CONST_CHAR() != nullptr) {
        memory_offset -= 4;
        variables.insert({get_new_tmp_varname(), VariableInfo(memory_offset, 4)});
    } else if (ctx->CONST_VOID() != nullptr) {
        memory_offset -= 8;
        variables.insert({get_new_tmp_varname(), VariableInfo(memory_offset, 8)});
    } else if (ctx->VARNAME() != nullptr) {
        if(variables.count(get_unique_var_name(ctx->VARNAME()->getText()))!=0){
            variables.at(get_unique_var_name(ctx->VARNAME()->getText())).used=true;
        }
        else{
            exit(UNDECLARED);
        }
    } else {
        return PROGRAMER_ERROR;
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

antlrcpp::Any SymbolGenVisitor::visitExpr_assignment(ifccParser::Expr_assignmentContext* ctx) {
    bool prev_declaration_mode = declaration_mode;
    declaration_mode = false;
    inExpr=true;
    visit(ctx->assignment_stmt());
    inExpr=false;
    declaration_mode = prev_declaration_mode;
    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitAssignment_add(ifccParser::Assignment_addContext* ctx) {
    visit(ctx->rvalue());
    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitAssignment_mult(ifccParser::Assignment_multContext* ctx) {
    visit(ctx->rvalue());
    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitPre_incrementation(ifccParser::Pre_incrementationContext* ctx) {
    variables.at(get_unique_var_name(ctx->lvalue()->VARNAME()->getText())).used=true;
    visit(ctx->lvalue());
    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitPost_incrementation(ifccParser::Post_incrementationContext* ctx) {
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    debug("Inserted #tmp" + std::to_string(tmp_index) + " (address " + std::to_string(memory_offset) + ")");
    variables.at(get_unique_var_name(ctx->lvalue()->VARNAME()->getText())).used=true;
    visit(ctx->lvalue());

    return GOOD;
}

antlrcpp::Any SymbolGenVisitor::visitExpr_function(ifccParser::Expr_functionContext* ctx) {
    visit(ctx->function_call());
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
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
        std::string unique_var_name = varname + "#" + currentFunction + "_" + std::to_string(block);
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
    return name + "#" + currentFunction + "_" + std::to_string(current_block);
}

antlrcpp::Any SymbolGenVisitor::visitExpr_parenthesis(ifccParser::Expr_parenthesisContext* ctx) {
    visit(ctx->expr());
    memory_offset -= 4;
    tmp_index++;
    variables.insert({"#tmp" + std::to_string(tmp_index), VariableInfo(memory_offset, 4)});
    return 0;
}
std::string SymbolGenVisitor::get_unique_var_name(std::string varname) {
    int block = current_block;
    while (block != -1) { // finds in blocks starting from the upper ones
        std::string unique_var_name = varname + "#" + currentFunction + "_" + std::to_string(block);
        if (variables.find(unique_var_name) != variables.end())
            return unique_var_name;
        block = blocks.at(block); // decrease block lvl
    }
    return "";
}