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
cmp_eq : variable_membre_gauche, variable_membre_droite, variable_destination
cmp_lt : variable_membre_gauche, variable_membre_droite, variable_destination
cmp_le : variable_membre_gauche, variable_membre_droite, variable_destination
ret : variable
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
                o << "mov" << size_to_letter(destination.size) << " %ebx," << to_string(destination.address) << "(%rbp)"
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
        case cmp_eq:
            break;
        case cmp_lt:
            break;
        case cmp_le:
            break;
        case ret: {
            if (target == Target::x86) {
                VariableInfo variable = bb->cfg->get_var_info(params[0]);

                o << "mov" << size_to_letter(variable.size) << " " << to_string(variable.address) << "(%rbp), %eax"
                  << endl;
                o << "leave" << endl;
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

CFG::CFG(std::map<std::string, VariableInfo> variables_, string entry_block_label_)
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