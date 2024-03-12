#include "CodeGenVisitor.h"


antlrcpp::Any CodeGenVisitor::visitExpr_parenthesis(ifccParser::Expr_parenthesisContext* ctx) {
	std::string var_name = visit(ctx->expr());
    int var_size = variables.at(var_name).size;
    int var_address  = variables.at(var_name).address;
    mov(std::to_string(var_address) + "(%rbp)", "%eax", var_size);
    
    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
    int tmp_var_size = variables.at(tmp_var_name).size;
    int tmp_var_address = variables.at(tmp_var_name).address;
    push_stack("%eax", tmp_var_address, tmp_var_size);
    return tmp_var_name;
}

antlrcpp::Any CodeGenVisitor::visitExpr_unaire(ifccParser::Expr_unaireContext* ctx) {
	std::string var_name = visit(ctx->expr());
    int var_size = variables.at(var_name).size;
    int var_address  = variables.at(var_name).address;

    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
    int tmp_var_size = variables.at(tmp_var_name).size;
    int tmp_var_address = variables.at(tmp_var_name).address;
    switch (ctx->OP->getText()[0])
    {
    case '~':
        mov(std::to_string(var_address) + "(%rbp)", "%eax", var_size);
        std::cout << "   notl %eax" << std::endl; 
        push_stack("%eax", tmp_var_address, tmp_var_size);
        break;
    case '-':
        mov(std::to_string(var_address) + "(%rbp)", "%eax", var_size);
        std::cout << "   negl %eax" << std::endl; 
        push_stack("%eax", tmp_var_address, tmp_var_size);
        break;
    default:
        break;
    }
    return tmp_var_name;
}

antlrcpp::Any CodeGenVisitor::visitExpr_mult(ifccParser::Expr_multContext* ctx) {
    std::string s= ctx->OP->getText();
    int a=this->visit(ctx->expr(0));
    int b=this->visit(ctx->expr(1));
    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
    int tmp_var_address = variables.at(tmp_var_name).address;
    if(s=="*"){
        std::cout<<"    movl "<<a<<"(%rbp), %eax"<<std::endl;
        std::cout<<"    movl "<<b<<"(%rbp), %ebx"<<std::endl;
        std::cout<<"	imull %ebx, %eax"<<std::endl;
        std::cout << "    movl %eax, "<<tmp_var_address<<"(%rbp)"<<std::endl;
    }
    else{
        std::cout<<"    movl "<<a<<"(%rbp), %eax"<<std::endl;
        std::cout<<"	cltd"<<std::endl;
        std::cout<<"	idivl "<<b<<"(%rbp)"<<std::endl;
        if(s=="/"){
            std::cout << "    movl %eax, "<<tmp_var_address<<"(%rbp)"<<std::endl;
        }
        else{
            std::cout << "    movl %edx, "<<tmp_var_address<<"(%rbp)"<<std::endl;
        }
    }
    return tmp_var_address;   
}

antlrcpp::Any CodeGenVisitor::visitExpr_add(ifccParser::Expr_addContext* ctx) {
    std::string s= ctx->OP->getText();
    int a=this->visit(ctx->expr(0));
    int b=this->visit(ctx->expr(1));
    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
    int tmp_var_address = variables.at(tmp_var_name).address;
    if(s=="+"){
        std::cout<<"    movl "<<a<<"(%rbp), %eax"<<std::endl;
        std::cout<<"    movl "<<b<<"(%rbp), %ebx"<<std::endl;
        std::cout<<"	addl %ebx, %eax"<<std::endl;
        std::cout << "    movl %eax, "<<tmp_var_address<<"(%rbp)"<<std::endl;
    }
    else{
        std::cout<<"    movl "<<a<<"(%rbp), %eax"<<std::endl;
        std::cout<<"    movl "<<b<<"(%rbp), %ebx"<<std::endl;
        std::cout<<"	subl %ebx, %eax"<<std::endl;
        std::cout << "    movl %eax, "<<tmp_var_address<<"(%rbp)"<<std::endl;
    }
    return tmp_var_address;	
}

antlrcpp::Any CodeGenVisitor::visitExpr_relational(ifccParser::Expr_relationalContext* ctx) {
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr_equality(ifccParser::Expr_equalityContext* ctx) {
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitBitwise(std::string l_var_name, char OP, std::string r_var_name) {
    int l_var_size = variables.at(l_var_name).size;
    int l_var_address  = variables.at(l_var_name).address;
    int r_var_size = variables.at(r_var_name).size;
    int r_var_address  = variables.at(r_var_name).address;

    tmp_index++;
    std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
    int tmp_var_size = variables.at(tmp_var_name).size;
    int tmp_var_address = variables.at(tmp_var_name).address;

    mov(std::to_string(l_var_address)+"(%rbp)", "%eax", 4);
    switch (OP)
    {
    case '&':
        std::cout << "    andl " << r_var_address << "(%rbp)" << ", %eax\n";
        break;
    
    case '^':
        std::cout << "    xorl " << r_var_address << "(%rbp)" << ", %eax\n";
        break;
    
    case '|':
        std::cout << "    orl " << r_var_address << "(%rbp)" << ", %eax\n";
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
    char OP = '&';
    visitBitwise(l_var_name, OP, r_var_name);
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr_xor(ifccParser::Expr_xorContext* ctx) {
    std::string l_var_name = visit(ctx->expr().at(0));
    std::string r_var_name = visit(ctx->expr().at(1));
    char OP = '^';
    visitBitwise(l_var_name, OP, r_var_name);
    return 0;
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr_or(ifccParser::Expr_orContext* ctx) {
    std::string l_var_name = visit(ctx->expr().at(0));
    std::string r_var_name = visit(ctx->expr().at(1));
    char OP = '|';
    visitBitwise(l_var_name, OP, r_var_name);
    return 0;
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr_lazy_and(ifccParser::Expr_lazy_andContext* ctx) {
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr_lazy_or(ifccParser::Expr_lazy_orContext* ctx) {
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr_atom(ifccParser::Expr_atomContext* ctx) {
	std::string var_name;
    if (ctx->CONST() != nullptr) {
        tmp_index++;
        var_name = "#tmp" + std::to_string(tmp_index); // we hope that it's the same #tmp number
        VariableInfo var = variables.at(var_name);
        push_stack("$" + ctx->CONST()->getText(), var.address, var.size);
    } else if (ctx->VARNAME() != nullptr) {
        var_name = ctx->VARNAME()->getText();
    }

    return var_name;
}







// antlrcpp::Any erogfhzr(ifccParser::ExprContext* ctx)
// {
//     // if(ctx->atomic_expr() != nullptr) {
//     //     std::string var_name = visit(ctx->atomic_expr());
//     //     return var_name;
//     // }

//     // if(ctx->OP != nullptr) {
//     //     std::string l_var_name = visit(ctx->expr().at(0));
//     //     std::string r_var_name = visit(ctx->expr().at(1));
//     //     int l_var_size = variables.at(l_var_name).size;
//     //     int l_var_address  = variables.at(l_var_name).address;
//     //     int r_var_size = variables.at(r_var_name).size;
//     //     int r_var_address  = variables.at(r_var_name).address;

//     //     tmp_index++;
//     //     std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
//     //     int tmp_var_size = variables.at(tmp_var_name).size;
//     //     int tmp_var_address = variables.at(tmp_var_name).address;

//     //     mov(std::to_string(l_var_address)+"(%rbp)", "%eax", 4);
//     //     switch (ctx->OP->getText()[0])
//     //     {
//     //     case '&':
//     //         std::cout << "    andl " << r_var_address << "(%rbp)" << ", %eax\n";
//     //         break;
        
//     //     case '^':
//     //         std::cout << "    xorl " << r_var_address << "(%rbp)" << ", %eax\n";
//     //         break;
        
//     //     case '|':
//     //         std::cout << "    orl " << r_var_address << "(%rbp)" << ", %eax\n";
//     //         break;
        
//     //     default:
//     //         break;
//     //     }

//     //     push_stack("%eax", tmp_var_address, tmp_var_size);
//     //     return tmp_var_name;
//     // } 


//     // if(ctx->OPU() != nullptr) {
//     //     std::string var_name = visit(ctx->expr().at(0));
//     //     int var_size = variables.at(var_name).size;
//     //     int var_address  = variables.at(var_name).address;

//     //     tmp_index++;
//     //     std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
//     //     int tmp_var_size = variables.at(tmp_var_name).size;
//     //     int tmp_var_address = variables.at(tmp_var_name).address;
//     //     switch (ctx->OPU()->getText()[0])
//     //     {
//     //     case '~':
//     //         mov(std::to_string(var_address) + "(%rbp)", "%eax", var_size);
//     //         std::cout << "   notl %eax" << std::endl; 
//     //         push_stack("%eax", tmp_var_address, tmp_var_size);
//     //         break;
//     //     case '-':
//     //         mov(std::to_string(var_address) + "(%rbp)", "%eax", var_size);
//     //         std::cout << "   negl %eax" << std::endl; 
//     //         push_stack("%eax", tmp_var_address, tmp_var_size);
//     //         break;
//     //     default:
//     //         break;
//     //     }
//     //     return tmp_var_name;
//     // }

//     std::string var_name = visit(ctx->expr().at(0));
//     int var_size = variables.at(var_name).size;
//     int var_address  = variables.at(var_name).address;
//     mov(std::to_string(var_address) + "(%rbp)", "%eax", var_size);
    
//     tmp_index++;
//     std::string tmp_var_name = "#tmp" + std::to_string(tmp_index);
//     int tmp_var_size = variables.at(tmp_var_name).size;
//     int tmp_var_address = variables.at(tmp_var_name).address;
//     push_stack("%eax", tmp_var_address, tmp_var_size);
//     return tmp_var_name;
// }





// antlrcpp::Any CodeGenVisitor::visitAtomic_expr(ifccParser::Atomic_exprContext* ctx) 
// {
//     std::string var_name;
//     if (ctx->CONST() != nullptr) {
//         tmp_index++;
//         var_name = "#tmp" + std::to_string(tmp_index); // we hope that it's the same #tmp number
//         VariableInfo var = variables.at(var_name);
//         push_stack("$" + ctx->CONST()->getText(), var.address, var.size);
//     } else if (ctx->VARNAME() != nullptr) {
//         var_name = ctx->VARNAME()->getText();
//     }

//     return var_name; // std::string
// }
