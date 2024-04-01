#include "IR.h"
#include <iostream>
#include <string>

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

string size_to_letter(int size_left, int size_right) {
	if (size_left == size_right)
		return size_to_letter(size_left);
	else if (size_left == 1 && size_right == 4) {
		return "sbl";
	} else
		exit(NOT_IMPLEMENTED_YET);
}

/*
ldconst : valeur_constante, variable_destination
copy : variable_source, variable_destination
cmp_const : variable_source ,constante, variable_destination
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
lazy_and : variable_membre_gauche, variable_membre_droite, variable_destination
lazy_or : variable_membre_gauche, variable_membre_droite, variable_destination
neg : variable, variable_destination
*/
void IRInstr::gen_asm(ostream& o, Target target) {
	switch (op) {
		case ldconst: {
			VariableInfo variable = bb->cfg->get_var_info(params[1]);
			if (variable.size == 4) {
			  if (target == Target::x86) {
				  o << "	mov" << size_to_letter(variable.size) << "	$" << params[0] << "," << to_string(variable.address)
					<< "(%rbp)" << endl;
			  } else if (variable.size == 1) {
				int numerical_value = int(params[0][1]);
				std::string value = std::to_string(numerical_value);
				o << "	mov" << size_to_letter(variable.size) << "	$" << value << "," << to_string(variable.address)
					<< "(%rbp)" << endl;
			  } else {
				exit(PROGRAMER_ERROR);
			  }
			}
			break;
		}
		case copy: {
			VariableInfo source = bb->cfg->get_var_info(params[0]);
			VariableInfo destination = bb->cfg->get_var_info(params[1]);
			std::cerr << params[1] << ":	" << destination.size << "	" << destination.address << std::endl;

			if (target == Target::x86) {
				o << "	mov" << size_to_letter(source.size) << "	" << to_string(source.address) << "(%rbp), %eax" << endl;
				o << "	mov" << size_to_letter(destination.size,source.size) << "	%eax," << to_string(destination.address) << "(%rbp)"
				  << endl;
			}
			break;
		}
		case copyIn: {
			VariableInfo source = bb->cfg->get_var_info(params[0]);
			string destination = params[1];

			if (target == Target::x86) {
				o << "	mov" << size_to_letter(source.size) << "	" << to_string(source.address) << "(%rbp), "<< destination << endl;
			}
			break;
		}
		case cmp_const: {
			VariableInfo destination = bb->cfg->get_var_info(params[2]);
			VariableInfo source = bb->cfg->get_var_info(params[0]);
			std::string constante;
			if (source.size == 4) {
			  constante = params[1];
			} else if (source.size == 1) {
			  int numerical_constante = int(params[1][1]);
			  constante = std::to_string(numerical_constante);
			} else {
			  exit(PROGRAMER_ERROR);
			}
			o << "	cmp" << size_to_letter(source.size) << "	$" << constante << ", " << to_string(source.address)
			  << "(%rbp)" << endl;
			o << "	sete " << to_string(destination.address) << "(%rbp)" << endl;
			break;
		}
		case add: {
			VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
			VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
			VariableInfo destination = bb->cfg->get_var_info(params[2]);

			if (target == Target::x86) {
				o << "	mov" << size_to_letter(membreGauche.size) << "	" << to_string(membreGauche.address)
				  << "(%rbp), %eax" << endl;
				o << "	mov" << size_to_letter(membreDroit.size) << "	" << to_string(membreDroit.address)
				  << "(%rbp), %edx" << endl;
				o << "	add" << size_to_letter(destination.size) << "	%edx, %eax" << endl;
				o << "	mov" << size_to_letter(destination.size) << "	%eax, " << to_string(destination.address)
				  << "(%rbp)" << endl;
			}
			break;
		}
		case sub: {
			VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
			VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
			VariableInfo destination = bb->cfg->get_var_info(params[2]);

			if (target == Target::x86) {
				o << "	mov" << size_to_letter(membreGauche.size) << "	" << to_string(membreGauche.address)
				  << "(%rbp), %eax" << endl;
				o << "	mov" << size_to_letter(membreDroit.size) << "	" << to_string(membreDroit.address)
				  << "(%rbp), %edx" << endl;
				o << "	sub" << size_to_letter(destination.size) << "	%edx, %eax" << endl;
				o << "	mov" << size_to_letter(destination.size) << "	%eax, " << to_string(destination.address)
				  << "(%rbp)" << endl;
			}
			break;
		}
		case mul: {
			VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
			VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
			VariableInfo destination = bb->cfg->get_var_info(params[2]);

			if (target == Target::x86) {
				o << "	mov" << size_to_letter(membreGauche.size) << "	" << to_string(membreGauche.address)
				  << "(%rbp), %eax" << endl;
				o << "	mov" << size_to_letter(membreDroit.size) << "	" << to_string(membreDroit.address)
				  << "(%rbp), %edx" << endl;
				o << "	imul" << size_to_letter(destination.size) << "	%edx, %eax" << endl;
				o << "	mov" << size_to_letter(destination.size) << "	%eax, " << to_string(destination.address)
				  << "(%rbp)" << endl;
			}
			break;
		}
		case l_not: {
			VariableInfo variableInitiale = bb->cfg->get_var_info(params[0]);
			VariableInfo destination = bb->cfg->get_var_info(params[1]);

			if (target == Target::x86) {
				o << "	cmp" << size_to_letter(variableInitiale.size) << "	$0, " << to_string(variableInitiale.address)
				  << "(%rbp)" << endl;
				o << "	sete %al" << endl;
				o << "	movzb" << size_to_letter(variableInitiale.size) << "	%al, %eax" << endl;
				o << "	mov" << size_to_letter(variableInitiale.size) << "	%eax, " << destination.address << "(%rbp)"
				  << endl;
			}
			break;
		}
		case cmp_eq: {
			VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
			VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
			VariableInfo destination = bb->cfg->get_var_info(params[2]);

			if (target == Target::x86) {
				o << "	mov" << size_to_letter(membreGauche.size) << "	" << to_string(membreGauche.address)
				  << "(%rbp), %eax" << endl;
				o << "	cmp" << size_to_letter(membreDroit.size) << "	" << to_string(membreDroit.address)
				  << "(%rbp), %eax" << endl;
				o << "	sete %al" << endl;
				o << "	movzb" << size_to_letter(destination.size) << "	%al, %eax" << endl;
				o << "	mov" << size_to_letter(destination.size) << "	%eax, " << to_string(destination.address)
				  << "(%rbp)" << endl;
			}
			break;
		}
		case cmp_ne: {
			VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
			VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
			VariableInfo destination = bb->cfg->get_var_info(params[2]);

			if (target == Target::x86) {
				o << "	mov" << size_to_letter(membreGauche.size) << "	" << to_string(membreGauche.address)
				  << "(%rbp), %eax" << endl;
				o << "	cmp" << size_to_letter(membreDroit.size) << "	" << to_string(membreDroit.address)
				  << "(%rbp), %eax" << endl;
				o << "	setne %al" << endl;
				o << "	movzb" << size_to_letter(destination.size) << "	%al, %eax" << endl;
				o << "	mov" << size_to_letter(destination.size) << "	%eax, " << to_string(destination.address)
				  << "(%rbp)" << endl;
			}
			break;
		}
		case cmp_lt: {
			VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
			VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
			VariableInfo destination = bb->cfg->get_var_info(params[2]);

			if (target == Target::x86) {
				o << "	mov" << size_to_letter(membreGauche.size) << "	" << to_string(membreGauche.address)
				  << "(%rbp), %eax" << endl;
				o << "	cmp" << size_to_letter(membreDroit.size) << "	" << to_string(membreDroit.address)
				  << "(%rbp), %eax" << endl;
				o << "	setl %al" << endl;
				o << "	movzb" << size_to_letter(destination.size) << "	%al, %eax" << endl;
				o << "	mov" << size_to_letter(destination.size) << "	%eax, " << to_string(destination.address)
				  << "(%rbp)" << endl;
			}
			break;
		}
		case cmp_le: {
			VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
			VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
			VariableInfo destination = bb->cfg->get_var_info(params[2]);

			if (target == Target::x86) {
				o << "	mov" << size_to_letter(membreGauche.size) << "	" << to_string(membreGauche.address)
				  << "(%rbp), %eax" << endl;
				o << "	cmp" << size_to_letter(membreDroit.size) << "	" << to_string(membreDroit.address)
				  << "(%rbp), %eax" << endl;
				o << "	setle %al" << endl;
				o << "	movzb" << size_to_letter(destination.size) << "	%al, %eax" << endl;
				o << "	mov" << size_to_letter(destination.size) << "	%eax, " << to_string(destination.address)
				  << "(%rbp)" << endl;
			}
			break;
		}
		case cmp_gt: {
			VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
			VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
			VariableInfo destination = bb->cfg->get_var_info(params[2]);

			if (target == Target::x86) {
				o << "	mov" << size_to_letter(membreGauche.size) << "	" << to_string(membreGauche.address)
				  << "(%rbp), %eax" << endl;
				o << "	cmp" << size_to_letter(membreDroit.size) << "	" << to_string(membreDroit.address)
				  << "(%rbp), %eax" << endl;
				o << "	setg %al" << endl;
				o << "	movzb" << size_to_letter(destination.size) << "	%al, %eax" << endl;
				o << "	mov" << size_to_letter(destination.size) << "	%eax, " << to_string(destination.address)
				  << "(%rbp)" << endl;
			}
			break;
		}
		case cmp_ge: {
			VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
			VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
			VariableInfo destination = bb->cfg->get_var_info(params[2]);

			if (target == Target::x86) {
				o << "	mov" << size_to_letter(membreGauche.size) << "	" << to_string(membreGauche.address)
				  << "(%rbp), %eax" << endl;
				o << "	cmp" << size_to_letter(membreDroit.size) << "	" << to_string(membreDroit.address)
				  << "(%rbp), %eax" << endl;
				o << "	setge %al" << endl;
				o << "	movzb" << size_to_letter(destination.size) << "	%al, %eax" << endl;
				o << "	mov" << size_to_letter(destination.size) << "	%eax, " << to_string(destination.address)
				  << "(%rbp)" << endl;
			}
			break;
		}
		case ret: {
			if (target == Target::x86) {
				VariableInfo variable = bb->cfg->get_var_info(params[0]);

				o << "	mov" << size_to_letter(variable.size) << "	" << to_string(variable.address) << "(%rbp), %eax"
				  << endl;
				o << "	leave" << endl;
				o << "	ret" << endl;
			}
			break;
		}
		case bitwise_and: {
			VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
			VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
			VariableInfo destination = bb->cfg->get_var_info(params[2]);

			if (target == Target::x86) {
				o << "	mov" << size_to_letter(membreGauche.size) << "	" << to_string(membreGauche.address)
				  << "(%rbp), %eax" << endl;
				o << "	and" << size_to_letter(destination.size) << "	" << to_string(membreDroit.address)
				  << "(%rbp), %eax" << endl;
				o << "	mov" << size_to_letter(destination.size) << "	%eax, " << to_string(destination.address)
				  << "(%rbp)" << endl;
			}
			break;
		}
		case bitwise_or: {
			VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
			VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
			VariableInfo destination = bb->cfg->get_var_info(params[2]);

			if (target == Target::x86) {
				o << "	mov" << size_to_letter(membreGauche.size) << "	" << to_string(membreGauche.address)
				  << "(%rbp), %eax" << endl;
				o << "	or" << size_to_letter(destination.size) << "	" << to_string(membreDroit.address) << "(%rbp), %eax"
				  << endl;
				o << "	mov" << size_to_letter(destination.size) << "	%eax, " << to_string(destination.address)
				  << "(%rbp)" << endl;
			}
			break;
		}
		case bitwise_xor: {
			VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
			VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
			VariableInfo destination = bb->cfg->get_var_info(params[2]);

			if (target == Target::x86) {
				o << "	mov" << size_to_letter(membreGauche.size) << "	" << to_string(membreGauche.address)
				  << "(%rbp), %eax" << endl;
				o << "	xor" << size_to_letter(destination.size) << "	" << to_string(membreDroit.address)
				  << "(%rbp), %eax" << endl;
				o << "	mov" << size_to_letter(destination.size) << "	%eax, " << to_string(destination.address)
				  << "(%rbp)" << endl;
			}
			break;
		}
		case bitwise_not: {
			VariableInfo varName = bb->cfg->get_var_info(params[0]);
			VariableInfo destination = bb->cfg->get_var_info(params[1]);

			if (target == Target::x86) {
				o << "	mov" << size_to_letter(varName.size) << "	" << to_string(varName.address) << "(%rbp), %eax"
				  << endl;
				o << "	not" << size_to_letter(destination.size) << "	%eax" << endl;
				o << "	mov" << size_to_letter(destination.size) << "	%eax, " << to_string(destination.address)
				  << "(%rbp)" << endl;
			}
			break;
		}
		case neg: {
			VariableInfo varName = bb->cfg->get_var_info(params[0]);
			VariableInfo destination = bb->cfg->get_var_info(params[1]);

			if (target == Target::x86) {
				o << "	mov" << size_to_letter(varName.size) << "	" << to_string(varName.address) << "(%rbp), %eax"
				  << endl;
				o << "	neg" << size_to_letter(destination.size) << "	%eax" << endl;
				o << "	mov" << size_to_letter(destination.size) << "	%eax, " << to_string(destination.address)
				  << "(%rbp)" << endl;
			}
			break;
		}
		case lazy_and: {
			VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
			VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
			// VariableInfo destination = bb->cfg->get_var_info(params[2]);
			string bloc1Name = "";
			string bloc2Name = "";

			if (target == Target::x86) {

				o << "	cmp" << size_to_letter(membreGauche.size) << "	$0, " << to_string(membreGauche.address)
				  << "(%rbp)" << endl;
				o << "	je	.L" << bloc1Name << endl;
				o << "	cmp" << size_to_letter(membreDroit.size) << "	$0, " << to_string(membreDroit.address) << "(%rbp)"
				  << endl;
				o << "	je	.L" << bloc1Name << endl;
				o << "	movl	$1, %eax" << endl;
				o << "	je	.L" << bloc2Name << endl;

				/*
				Dans L2
				o << "movl $0, %eax" << endl;

				Dans L4
				on continue
				*/
			}
			break;
		}
		case lazy_or: {
			VariableInfo membreGauche = bb->cfg->get_var_info(params[0]);
			VariableInfo membreDroit = bb->cfg->get_var_info(params[1]);
			VariableInfo destination = bb->cfg->get_var_info(params[2]);

			if (target == Target::x86) {
				o << "	mov" << size_to_letter(membreGauche.size) << "	" << to_string(membreGauche.address)
				  << "(%rbp), %eax" << endl;
				o << "	or" << size_to_letter(destination.size) << "	" << to_string(membreDroit.address) << "(%rbp), %eax"
				  << endl;
				o << "	mov" << size_to_letter(destination.size) << "	%eax, " << to_string(destination.address)
				  << "(%rbp)" << endl;
			}
			break;
		}
		case call: {
		  string fctName = params[0];

		  if (target == Target::x86) {
			  o << "	call	"<<fctName<<endl;
		  }
		  break;
		}
		case mov_eax: {
			exit(NOT_IMPLEMENTED_YET);
		}
		case mov_from_eax: {
			exit(NOT_IMPLEMENTED_YET);
		}
		case rmem: {
			exit(NOT_IMPLEMENTED_YET);
		}
		case wmem: {
			exit(NOT_IMPLEMENTED_YET);
		}
		default: {
			exit(PROGRAMER_ERROR);
		}
	}
}

BasicBlock::BasicBlock(CFG* cfg, string entry_label) : cfg(cfg), label(entry_label) {
	exit_false = nullptr;
	exit_true = nullptr;
}

void BasicBlock::gen_asm(ostream& o, Target target) {
	if (target == Target::x86) {
		o << endl << label << ":" << endl;
		for (auto instruction : instrs) {
			instruction->gen_asm(o, target);
		}

		if (exit_false == nullptr && exit_true != nullptr) {
			// unconditional jmp to the exit_true branch
			o << "	jmp	" << exit_true->label << endl;
		} else if (exit_false != nullptr && exit_true == nullptr) {
			// unconditional jmp to the exit_true branch
			o << "	jmp	" << exit_false->label << endl;
		} else if (exit_false != nullptr && exit_true != nullptr) {
			// jmp to exit_true if test_var_name, otherwise jmp to exit_false
			VariableInfo testVar = cfg->get_var_info(test_var_name);
			o << "	mov" << size_to_letter(testVar.size) << "	" << to_string(testVar.address) << "(%rbp), %eax" << endl;
			o << "	cmp" << size_to_letter(testVar.size) << "	$0, %eax" << endl;
			o << "	jz	" << exit_false->label << endl;
			o << "	jmp	" << exit_true->label << endl;
		}
	}
}

void BasicBlock::add_IRInstr(IRInstr::Operation op, Type t, vector<string> params) {
	IRInstr* instruction = new IRInstr(this, op, t, params);
	instrs.push_back(instruction);
}

CFG::CFG(std::unordered_map<std::string, VariableInfo>& variables_, string entry_block_label_)
	: variables(variables_), entry_block_label(entry_block_label_), nextBBnumber(0) {}

void CFG::add_bb(BasicBlock* bb) {
	bbs.push_back(bb);
	// current_bb = bb;
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
		o << "	pushq	%rbp" << endl;				// Save the old base pointer
		o << "	movq	%rsp, %rbp" << endl;		   // Set up a new base pointer
		o << "	subq	$"<<(-memoryUse/16+2)*16<<", %rsp"<<endl;		//Set up potential function call
		o << "	jmp	" << entry_block_label << endl; // Jump to entry block
	}
}

void CFG::gen_asm_epilogue(ostream& o, Target target) {
	if (target == Target::x86) {
	}
}

void CFG::add_to_symbol_table(string name, VariableInfo t) { variables[name] = t; }

string CFG::new_BB_name() {
	nextBBnumber++;
	return "BB_" + to_string(nextBBnumber);
}

VariableInfo CFG::get_var_info(string name) { return variables[name]; }




