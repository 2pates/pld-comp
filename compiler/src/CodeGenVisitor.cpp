#include "CodeGenVisitor.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext* ctx) {
    std::cout << ".globl main\n";
    std::cout << "main: \n";

    for (auto instr : ctx->instruction()) {
        this->visit(instr);
    }

    this->visit(ctx->return_stmt());
    std::cout << "    ret\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitInstruction(ifccParser::InstructionContext* ctx) {
    if (ctx->assignment_stmt() != nullptr)
        this->visit(ctx->assignment_stmt());
    if (ctx->declare_stmt() != nullptr)
        this->visit(ctx->declare_stmt());
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext* ctx) {
    int retval = stoi(ctx->CONST()->getText());

    std::cout << "    movl $" << retval << ", %eax\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx) { return 0; }

antlrcpp::Any CodeGenVisitor::visitAssignment_stmt(ifccParser::Assignment_stmtContext* ctx) {
    if (variables.find(ctx->lvalue()->getText()) != variables.end()) {
        VariableInfo varInfo = variables.at(ctx->lvalue()->getText());
        int address = varInfo.address;
        int size = varInfo.size;
        std::string name = visit(ctx->rvalue());
        if (variables.find(name) != variables.end()) {
            int r_addr = variables.at(name).address;
            push_stack(r_addr, address);
            return 0;
        } else {
            std::cerr << "Programmer error" << std::endl;
            return PROGRAMER_ERROR;
        }
    } else {
        std::cerr << "Undeclared error" << std::endl;
        return UNDECLARED;
    }
}

antlrcpp::Any CodeGenVisitor::visitRvalue(ifccParser::RvalueContext* ctx) {
    std::string var_name;
    if (ctx->CONST() != nullptr) {
        tmp_index++;
        var_name = "#tmp" + std::to_string(tmp_index); // we hope that it's the same #tmp number
        VariableInfo var = variables.at(var_name);
        push_stack("$" + ctx->CONST()->getText(), var.address, var.size);
    } else if (ctx->VARNAME() != nullptr) {
        var_name = ctx->VARNAME()->getText();
    }
    return var_name; // std::string
}

antlrcpp::Any CodeGenVisitor::visitLvalue(ifccParser::LvalueContext* ctx) { return 0; }

int CodeGenVisitor::push_stack(int source, int dest) {
    std::cout << "    movq ";
    std::cout << source << "(%rbp)"
              << ", ";
    std::cout << dest << "(%rbp)"
              << "\n";
    return 0;
}

int CodeGenVisitor::push_stack(std::string source, int dest, int size) {
    switch (size) {
        case 8:
            std::cout << "    movq ";
            break;
        case 4:
            std::cout << "    movl ";
            break;
        case 1:
            std::cout << "    movb ";
            break;
        default:
            break;
    }
    // std::cout << "$" << source << ", ";
    std::cout << source << ", ";
    std::cout << dest << "(%rbp)"
              << "\n";
    return 0;
}
