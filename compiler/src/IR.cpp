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
*/
IRInstr::gen_asm(ostream& o, Target target) {
    switch (op) {
        case ldconst: 
            VariableInfo variable = bb->cfg->get_var_info(params[1]);
            if (target == Target::x86) {
                o << "mov" << size_to_letter(variable.size) << " $" << params[0] << "," << to_string(variable.address) << "(%rbp)" << endl;
            }
            break;
        case copy:
            VariableInfo source = bb->cfg->get_var_info(params[0]);
            VariableInfo destination = bb->cfg->get_var_info(params[1]);

            if(target == Target::x86)
            {
                o << "mov" << size_to_letter(membreGauche.size) << " " << to_string(membreGauche.address) << "(%rbp), %eax" << endl;
                o << "mov" << size_to_letter(membreDroit.size) << " %ebx," << to_string(membreDroit.address) << "(%rbp)" << endl;
            }
            break;
        case add:
            VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
            VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
            VariableInfo destination = bb->cfg->get_var_info(params[2]);

            if(target == Target::x86)
            {
                o << "mov" << size_to_letter(membreGauche.size) << " " << to_string(membreGauche.address) << "(%rbp), %eax" << endl;
                o << "mov" << size_to_letter(membreDroit.size) << " " << to_string(membreDroit.address) << "(%rbp), %ebx" << endl;
                o << "add" << size_to_letter(destination.size) << " %ebx, %eax" << endl;
                o << "mov" << size_to_letter(destination.size) << " %eax, " << to_string(destination.address) << "(%rbp)" << endl;
            }
            break;
    }
}