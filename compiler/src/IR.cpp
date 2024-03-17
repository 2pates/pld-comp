#include "IR.h"
#include <iostream>

using namespace std;

IRInstr::IRInstr(BasicBlock* bb_, Operation op, Type t, vector<string> params)
    : bb(bb_), op(op), t(t), params(params) {}

string size_to_letter(int size) {
    switch (size) {
        case 1:
            return "b";
        case 4:
            return "l";
        case 8:
            return "q";
        default:
            return nullptr;
    }
}

/*
ldconst : valeur_constante, variable_destination
copy : variable_source, variable_destination
add : variable_membre_gauche, variable_membre_droite, variable_destination
sub : variable_membre_gauche, variable_membre_droite, variable_destination
mul : variable_membre_gauche, variable_membre_droite, variable_destination
l_not : variable_initiale, varibale_destination
cmp_eq : variable_membre_gauche, variable_membre_droite, variable_destination
cmp_ne : variable_membre_gauche, variable_membre_droite, variable_destination
cmp_lt : variable_membre_gauche, variable_membre_droite, variable_destination
cmp_le : variable_membre_gauche, variable_membre_droite, variable_destination
cmp_gt : variable_membre_gauche, variable_membre_droite, variable_destination
cmp_ge : variable_membre_gauche, variable_membre_droite, variable_destination
ret : variable
bitwise_and : variable_membre_gauche, variable_membre_droite, variable_destination
bitwise_or : variable_membre_gauche, variable_membre_droite, variable_destination
bitwise_xor : variable_membre_gauche, variable_membre_droite, variable_destination
bitwise_not : variable, variable_destination
neg : variable, variable_destination
*/
void IRInstr::gen_asm(ostream& o, Target target) {
    switch (op) {
        case ldconst: {
            VariableInfo variable = bb->cfg->get_var_info(params[1]);
            if (target == Target::x86) {
                o << "mov" << size_to_letter(variable.size) << " $" << params[0] << "," << to_string(variable.address)
                  << "(%rbp)" << endl;
            }
            break;
        }
        case copy: {
            VariableInfo source = bb->cfg->get_var_info(params[0]);
            VariableInfo destination = bb->cfg->get_var_info(params[1]);

            if (target == Target::x86) {
                o << "mov" << size_to_letter(source.size) << " " << to_string(source.address) << "(%rbp), %eax" << endl;
                o << "mov" << size_to_letter(destination.size) << " %eax," << to_string(destination.address) << "(%rbp)"
                  << endl;
            }
            break;
        }
        case add: {
            VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
            VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
            VariableInfo destination = bb->cfg->get_var_info(params[2]);

            if (target == Target::x86) {
                o << "mov" << size_to_letter(membreGauche.size) << " " << to_string(membreGauche.address)
                  << "(%rbp), %eax" << endl;
                o << "mov" << size_to_letter(membreDroit.size) << " " << to_string(membreDroit.address)
                  << "(%rbp), %ebx" << endl;
                o << "add" << size_to_letter(destination.size) << " %ebx, %eax" << endl;
                o << "mov" << size_to_letter(destination.size) << " %eax, " << to_string(destination.address)
                  << "(%rbp)" << endl;
            }
            break;
        }
        case div: {
            // Assuming params[0] and params[1] hold the variable names/identifiers for the dividend and divisor
            // and that params[2] holds the destination variable identifier.
            VariableInfo dividend = bb->cfg->get_var_info(params[0]);
            VariableInfo divisor = bb->cfg->get_var_info(params[1]);
            VariableInfo destination = bb->cfg->get_var_info(params[2]);

            if (target == Target::x86) {
                // Load the dividend into eax. In your provided assembly, variables are directly accessed via their stack addresses.
                o << "movl " << to_string(dividend.address) << "(%rbp), %eax" << endl;
                
                // Prepare %edx:%eax for division by sign-extending %eax into %edx. This is analogous to the 'cltd' instruction.
                o << "cltd" << endl;
                
                // Perform the division. The 'idivl' instruction divides %edx:%eax by the divisor, with the quotient stored in %eax.
                // The divisor is accessed directly from its stack address.
                o << "idivl " << to_string(divisor.address) << "(%rbp)" << endl;
                
                // Store the quotient (result of the division) at the destination variable's stack address.
                o << "movl %eax, " << to_string(destination.address) << "(%rbp)" << endl;
            }
            break;
        }


        case mod: {
            VariableInfo membreGauche = bb->cfg->get_var_info(params[0]); // Operand for dividend
            VariableInfo membreDroit = bb->cfg->get_var_info(params[1]); // Operand for divisor
            VariableInfo destination = bb->cfg->get_var_info(params[2]); // Result of modulo

            if (target == Target::x86) {
                // Load the dividend into eax
                o << "mov" << size_to_letter(membreGauche.size) << " " << to_string(membreGauche.address) << "(%rbp), %eax" << endl;
                // Sign-extend eax into edx to prepare edx:eax for division
                o << "cdq" << endl;
                // Load the divisor into a register, using ebx for example
                o << "mov" << size_to_letter(membreDroit.size) << " " << to_string(membreDroit.address) << "(%rbp), %ebx" << endl;
                // Perform the division with idiv; quotient in eax, remainder in edx
                o << "idivl %ebx" << endl;
                // For modulo, the interest is in the remainder, which is now in edx. Move it to the destination.
                o << "mov" << size_to_letter(destination.size) << " %edx, " << to_string(destination.address) << "(%rbp)" << endl;
            }
            break;
        }


        case sub: {
            VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
            VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
            VariableInfo destination = bb->cfg->get_var_info(params[2]);

            if (target == Target::x86) {
                o << "mov" << size_to_letter(membreGauche.size) << " " << to_string(membreGauche.address)
                  << "(%rbp), %eax" << endl;
                o << "mov" << size_to_letter(membreDroit.size) << " " << to_string(membreDroit.address)
                  << "(%rbp), %ebx" << endl;
                o << "sub" << size_to_letter(destination.size) << " %ebx, %eax" << endl;
                o << "mov" << size_to_letter(destination.size) << " %eax, " << to_string(destination.address)
                  << "(%rbp)" << endl;
            }
            break;
        }
        case mul: {
            VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
            VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
            VariableInfo destination = bb->cfg->get_var_info(params[2]);

            if (target == Target::x86) {
                o << "mov" << size_to_letter(membreGauche.size) << " " << to_string(membreGauche.address)
                  << "(%rbp), %eax" << endl;
                o << "mov" << size_to_letter(membreDroit.size) << " " << to_string(membreDroit.address)
                  << "(%rbp), %ebx" << endl;
                o << "imul" << size_to_letter(destination.size) << " %ebx, %eax" << endl;
                o << "mov" << size_to_letter(destination.size) << " %eax, " << to_string(destination.address)
                  << "(%rbp)" << endl;
            }
            break;
        }
        case l_not: {
            VariableInfo variableInitiale = bb->cfg->get_var_info(params[0]);
            VariableInfo destination = bb->cfg->get_var_info(params[1]);

            if (target == Target::x86) {
                o << "cmp" << size_to_letter(variableInitiale.size) << " $0, " << to_string(variableInitiale.address)
                  << "(%rbp)" << endl;
                o << "sete %al" << endl;
                o << "movzb" << size_to_letter(variableInitiale.size) << " %al, %eax" << endl;
                o << "mov" << size_to_letter(variableInitiale.size) << " %eax, " << destination.size << "(%rbp)"
                  << endl;
            }
            break;
        }
        case cmp_eq: {
            VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
            VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
            VariableInfo destination = bb->cfg->get_var_info(params[2]);

            if (target == Target::x86) {
                o << "mov" << size_to_letter(membreGauche.size) << " " << to_string(membreGauche.address)
                  << "(%rbp), %eax" << endl;
                o << "cmp" << size_to_letter(membreDroit.size) << " " << to_string(membreDroit.address)
                  << "(%rbp), %eax" << endl;
                o << "sete %al" << endl;
                o << "movzb" << size_to_letter(destination.size) << " %al, %eax" << endl;
                o << "mov" << size_to_letter(destination.size) << " %eax, " << to_string(destination.address)
                  << "(%rbp)" << endl;
            }
            break;
        }
        case cmp_ne: {
            VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
            VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
            VariableInfo destination = bb->cfg->get_var_info(params[2]);

            if (target == Target::x86) {
                o << "mov" << size_to_letter(membreGauche.size) << " " << to_string(membreGauche.address)
                  << "(%rbp), %eax" << endl;
                o << "cmp" << size_to_letter(membreDroit.size) << " " << to_string(membreDroit.address)
                  << "(%rbp), %eax" << endl;
                o << "setne %al" << endl;
                o << "movzb" << size_to_letter(destination.size) << " %al, %eax" << endl;
                o << "mov" << size_to_letter(destination.size) << " %eax, " << to_string(destination.address)
                  << "(%rbp)" << endl;
            }
            break;
        }
        case cmp_lt: {
            VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
            VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
            VariableInfo destination = bb->cfg->get_var_info(params[2]);

            if (target == Target::x86) {
                o << "mov" << size_to_letter(membreGauche.size) << " " << to_string(membreGauche.address)
                  << "(%rbp), %eax" << endl;
                o << "cmp" << size_to_letter(membreDroit.size) << " " << to_string(membreDroit.address)
                  << "(%rbp), %eax" << endl;
                o << "setl %al" << endl;
                o << "movzb" << size_to_letter(destination.size) << " %al, %eax" << endl;
                o << "mov" << size_to_letter(destination.size) << " %eax, " << to_string(destination.address)
                  << "(%rbp)" << endl;
            }
            break;
        }
        case cmp_le: {
            VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
            VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
            VariableInfo destination = bb->cfg->get_var_info(params[2]);

            if (target == Target::x86) {
                o << "mov" << size_to_letter(membreGauche.size) << " " << to_string(membreGauche.address)
                  << "(%rbp), %eax" << endl;
                o << "cmp" << size_to_letter(membreDroit.size) << " " << to_string(membreDroit.address)
                  << "(%rbp), %eax" << endl;
                o << "setle %al" << endl;
                o << "movzb" << size_to_letter(destination.size) << " %al, %eax" << endl;
                o << "mov" << size_to_letter(destination.size) << " %eax, " << to_string(destination.address)
                  << "(%rbp)" << endl;
            }
            break;
        }
        case cmp_gt: {
            VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
            VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
            VariableInfo destination = bb->cfg->get_var_info(params[2]);

            if (target == Target::x86) {
                o << "mov" << size_to_letter(membreGauche.size) << " " << to_string(membreGauche.address)
                  << "(%rbp), %eax" << endl;
                o << "cmp" << size_to_letter(membreDroit.size) << " " << to_string(membreDroit.address)
                  << "(%rbp), %eax" << endl;
                o << "setg %al" << endl;
                o << "movzb" << size_to_letter(destination.size) << " %al, %eax" << endl;
                o << "mov" << size_to_letter(destination.size) << " %eax, " << to_string(destination.address)
                  << "(%rbp)" << endl;
            }
            break;
        }
        case cmp_ge: {
            VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
            VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
            VariableInfo destination = bb->cfg->get_var_info(params[2]);

            if (target == Target::x86) {
                o << "mov" << size_to_letter(membreGauche.size) << " " << to_string(membreGauche.address)
                  << "(%rbp), %eax" << endl;
                o << "cmp" << size_to_letter(membreDroit.size) << " " << to_string(membreDroit.address)
                  << "(%rbp), %eax" << endl;
                o << "setge %al" << endl;
                o << "movzb" << size_to_letter(destination.size) << " %al, %eax" << endl;
                o << "mov" << size_to_letter(destination.size) << " %eax, " << to_string(destination.address)
                  << "(%rbp)" << endl;
            }
            break;
        }
        case ret: {
            if (target == Target::x86) {
                VariableInfo variable = bb->cfg->get_var_info(params[0]);

                o << "mov" << size_to_letter(variable.size) << " " << to_string(variable.address) << "(%rbp), %eax"
                  << endl;
                o << "leave" << endl;
            }
            break;
        }
        case bitwise_and: {
            VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
            VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
            VariableInfo destination = bb->cfg->get_var_info(params[2]);

            if (target == Target::x86) {
                o << "mov" << size_to_letter(membreGauche.size) << " " << to_string(membreGauche.address)
                  << "(%rbp), %eax" << endl;
                o << "and" << size_to_letter(destination.size) << " " << to_string(membreDroit.address)
                  << "(%rbp), %eax" << endl;
                o << "mov" << size_to_letter(destination.size) << " %eax, " << to_string(destination.address)
                  << "(%rbp)" << endl;
            }
            break;
        }
        case bitwise_or: {
            VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
            VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
            VariableInfo destination = bb->cfg->get_var_info(params[2]);

            if (target == Target::x86) {
                o << "mov" << size_to_letter(membreGauche.size) << " " << to_string(membreGauche.address)
                  << "(%rbp), %eax" << endl;
                o << "or" << size_to_letter(destination.size) << " " << to_string(membreDroit.address)
                  << "(%rbp), %eax" << endl;
                o << "mov" << size_to_letter(destination.size) << " %eax, " << to_string(destination.address)
                  << "(%rbp)" << endl;
            }
            break;
        }
        case bitwise_xor: {
            VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
            VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
            VariableInfo destination = bb->cfg->get_var_info(params[2]);

            if (target == Target::x86) {
                o << "mov" << size_to_letter(membreGauche.size) << " " << to_string(membreGauche.address)
                  << "(%rbp), %eax" << endl;
                o << "xor" << size_to_letter(destination.size) << " " << to_string(membreDroit.address)
                  << "(%rbp), %eax" << endl;
                o << "mov" << size_to_letter(destination.size) << " %eax, " << to_string(destination.address)
                  << "(%rbp)" << endl;
            }
            break;
        }
        case bitwise_not: {
            VariableInfo varName = bb->cfg->get_var_info(params[0]);
            VariableInfo destination = bb->cfg->get_var_info(params[1]);

            if (target == Target::x86) {
                o << "mov" << size_to_letter(varName.size) << " " << to_string(varName.address)
                  << "(%rbp), %eax" << endl;
                o << "not" << size_to_letter(destination.size) << " %eax" << endl;
                o << "mov" << size_to_letter(destination.size) << " %eax, " << to_string(destination.address)
                  << "(%rbp)" << endl;
            }
            break;
        }
        case neg: {
            VariableInfo varName = bb->cfg->get_var_info(params[0]);
            VariableInfo destination = bb->cfg->get_var_info(params[1]);

            if (target == Target::x86) {
                o << "mov" << size_to_letter(varName.size) << " " << to_string(varName.address)
                  << "(%rbp), %eax" << endl;
                o << "neg" << size_to_letter(destination.size) << " %eax" << endl;
                o << "mov" << size_to_letter(destination.size) << " %eax, " << to_string(destination.address)
                  << "(%rbp)" << endl;
            }
            break;
        
        }
    }
}

BasicBlock::BasicBlock(CFG* cfg, string entry_label) : cfg(cfg), label(entry_label) {}

void BasicBlock::gen_asm(ostream& o, Target target) {
    if (target == Target::x86) {
        o << endl << label << ":" << endl;
        for (auto instruction : instrs) {
            instruction->gen_asm(o, target);
        }

        if (exit_false == nullptr && exit_true != nullptr) {
            // unconditional jmp to the exit_true branch
            o << "jmp " << exit_true->label << endl;
        } else if (exit_false != nullptr && exit_true == nullptr) {
            // unconditional jmp to the exit_true branch
            o << "jmp " << exit_false->label << endl;
        } else if (exit_false != nullptr && exit_true != nullptr) {
            // jmp to exit_true if test_var_name, otherwise jmp to exit_false
            VariableInfo testVar = cfg->get_var_info(test_var_name);
            o << "mov" << size_to_letter(testVar.size) << " " << to_string(testVar.address) << "(%rbp), %eax" << endl;
            o << "cmp %eax, $1";
            o << "jnz " << exit_true->label << endl;
            o << "jmp " << exit_false->label << endl;
        }
    }
}

void BasicBlock::add_IRInstr(IRInstr::Operation op, Type t, vector<string> params) {
    IRInstr* instruction = new IRInstr(this, op, t, params);
    instrs.push_back(instruction);
}

CFG::CFG(std::map<std::string, VariableInfo>& variables_, string entry_block_label_)
    : variables(variables_), entry_block_label(entry_block_label_), nextBBnumber(0) {}

void CFG::add_bb(BasicBlock* bb) {
    bbs.push_back(bb);
    current_bb = bb;
}

void CFG::gen_asm(ostream& o, Target target) {
    gen_asm_prologue(o, target);
    for (auto basicBlock : bbs) {
        basicBlock->gen_asm(o, target);
    }
    gen_asm_epilogue(o, target);
}

void CFG::gen_asm_prologue(ostream& o, Target target) {
    if (target == Target::x86) {
        o << ".globl main" << endl;
        o << "main:" << endl;
        o << "pushq %rbp" << endl;                // Save the old base pointer
        o << "movq %rsp, %rbp" << endl;           // Set up a new base pointer
        o << "jmp " << entry_block_label << endl; // Jump to entry block
    }
}

void CFG::gen_asm_epilogue(ostream& o, Target target) {
    if (target == Target::x86) {
        o << "ret" << endl;
    }
}

void CFG::add_to_symbol_table(string name, VariableInfo t) { variables[name] = t; }

string CFG::new_BB_name() {
    nextBBnumber++;
    return "bb_" + to_string(nextBBnumber);
}

VariableInfo CFG::get_var_info(string name) { return variables[name]; }