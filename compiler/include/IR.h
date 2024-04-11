#ifndef IR_H
#define IR_H

#include <initializer_list>
#include <iostream>
#include <string>
#include <vector>

// Declarations from the parser
#include "SymbolGenVisitor.h"

#include "Type.h"
using namespace std;

// Forward declarations
class BasicBlock;
class CFG;

// Define the target architecture
typedef enum { x86 } Target;

/**
 * @brief The class for one 3-address instruction.
 */
class IRInstr {
public:
    /**
     * @brief The operation types for IR instructions.
     */
    typedef enum {
        ldconst,     /**< Load a constant value into a register. */
        copy,        /**< Copy the value from one register to another. */
        cmp_const,   /**< Compare a register value with a constant. */
        add,         /**< Add two register values. */
        add_const,   /**< Add a constant value to a register value. */
        sub,         /**< Subtract one register value from another. */
        sub_const,   /**< Subtract a constant value from a register value. */
        mul,         /**< Multiply two register values. */
        div,         /**< Divide one register value by another. */
        mod,         /**< Get the modulus of one register value by another. */
        call,        /**< Call a function. */
        cmp_eq,      /**< Compare two register values for equality. */
        cmp_ne,      /**< Compare two register values for inequality. */
        cmp_lt,      /**< Compare two register values: less than. */
        cmp_le,      /**< Compare two register values: less than or equal. */
        cmp_gt,      /**< Compare two register values: greater than. */
        cmp_ge,      /**< Compare two register values: greater than or equal. */
        copyIn,      /**< Copy value into a register from memory. */
        ret,         /**< Return from function. */
        copyOut,     /**< Copy value out from register to memory. */
        bitwise_and, /**< Perform bitwise AND operation. */
        bitwise_or,  /**< Perform bitwise OR operation. */
        bitwise_xor, /**< Perform bitwise XOR operation. */
        bitwise_not, /**< Perform bitwise NOT operation. */
        neg,         /**< Negate a register value. */
        jump,        /**< Jump to another instruction. */
        l_not,       /**< Perform logical NOT operation. */
        lazy_and,    /**< Perform lazy AND operation. */
        lazy_or,     /**< Perform lazy OR operation. */
        startfct     /**< Start of a function. */
    } Operation;

    /**
     * @brief Array representing register names used in x86 assembly.
     *
     * The array contains the names of the registers used in x86 assembly instructions.
     * These registers are commonly used in the IR instructions for code generation.
     */
    string repList[7] = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d", "%eax"};

    /**
     * @brief Constructor for IRInstr class.
     *
     * @param bb_ Pointer to the BasicBlock to which this instruction belongs.
     * @param op Operation type.
     * @param t Type of the operands.
     * @param params Vector of instruction parameters.
     */
    IRInstr(BasicBlock* bb_, Operation op, Type t, std::vector<std::string> params);

    /**
     * @brief Generate x86 assembly code for this IR instruction.
     *
     * @param o Output stream to write the assembly code.
     * @param target Target architecture.
     */
    void gen_asm(std::ostream& o, Target target);

private:
    BasicBlock* bb;                  /**< The BasicBlock this instruction belongs to, providing a pointer to the CFG. */
    Operation op;                    /**< Operation type. */
    Type t;                          /**< Type of the operands. */
    std::vector<std::string> params; /**< Vector of instruction parameters. */
};

/**
 * @brief The class for a basic block.
 *
 * A few important comments:
 * - IRInstr has no jump instructions.
 * - cmp_* instructions behave as arithmetic two-operand instructions (add or multiply),
 *   returning a boolean value (as an int).
 *
 * Assembly jumps are generated as follows:
 * - BasicBlock::gen_asm() first calls IRInstr::gen_asm() on all its instructions, and then
 *   - if exit_true is nullptr, the epilogue is generated.
 *   - else if exit_false is nullptr, an unconditional jump to the exit_true branch is generated.
 *   - else (we have two successors, hence a branch), an instruction comparing the value of test_var_name to true is generated,
 *     followed by a conditional branch to the exit_false branch, followed by an unconditional branch to the exit_true branch.
 *
 * The attribute test_var_name itself is defined when converting the if, while, etc. of the AST to IR.
 *
 * Possible optimization:
 * - A cmp_* comparison instruction, if it is the last instruction of its block, generates an actual assembly comparison
 *   followed by a conditional jump to the exit_false branch.
 */
class BasicBlock {
public:
    /**
     * @brief Constructor for BasicBlock class.
     *
     * @param cfg Pointer to the Control Flow Graph.
     * @param entry_label Label of the basic block.
     * @param parent_test Pointer to the parent test block (used for "continue" statement).
     * @param next_block Pointer to the next basic block.
     */
    BasicBlock(CFG* cfg, std::string entry_label, BasicBlock* parent_test = nullptr, BasicBlock* next_block = nullptr);

    /**
     * @brief Generate x86 assembly code for this basic block.
     *
     * @param o Output stream to write the assembly code.
     * @param target Target architecture.
     */
    void gen_asm(std::ostream& o, Target target);

    /**
     * @brief Add an IR instruction to this basic block.
     *
     * @param op Operation type.
     * @param t Type of the operands.
     * @param params Vector of instruction parameters.
     */
    void add_IRInstr(IRInstr::Operation op, Type t, std::vector<std::string> params);

    BasicBlock* exit_true; /**< Pointer to the next basic block, true branch. If nullptr, return from procedure. */
    BasicBlock*
        exit_false;    /**< Pointer to the next basic block, false branch. If nullptr, the basic block ends with an unconditional jump. */
    CFG* cfg;          /**< Pointer to the CFG where this block belongs. */
    std::string label; /**< Label of the BB, also will be the label in the generated code. */
    std::vector<IRInstr*> instrs; /**< The instructions themselves. */
    std::string test_var_name;    /**< When generating IR code for an if(expr) or while(expr) etc., store here the name of the variable that
                                     holds the value of expr. */
    BasicBlock* parent_test;      /**< Test block in case we are in a loop: used for "continue" statement. */
    BasicBlock* next_block;       /**< Pointer to the next basic block if we are in a branch. */
protected:
};

/**
 * @brief The class for the control flow graph, also includes the symbol table.
 *
 * A few important comments:
 * - The entry block is the one with the same label as the AST function name.
 *   (it could be the first of bbs, or it could be defined by an attribute value)
 * - The exit block is the one with both exit pointers equal to nullptr.
 *   (again it could be identified in a more explicit way)
 */
class CFG {
    friend class CodeGenVisitor;

public:
    /**
     * @brief Constructor for CFG class.
     *
     * @param variables_ Reference to the symbol table (variables).
     * @param entry_block_label_ Label of the entry block.
     */
    CFG(std::unordered_map<std::string, VariableInfo>& variables_, std::string entry_block_label_);

    /**
     * @brief Add a basic block to the CFG.
     *
     * @param bb Pointer to the basic block to add.
     */
    void add_bb(BasicBlock* bb);

    /**
     * @brief Generate x86 assembly code for the CFG.
     *
     * @param o Output stream to write the assembly code.
     * @param target Target architecture.
     */
    void gen_asm(std::ostream& o, Target target);

    /**
     * @brief Convert IR register or input variable to assembly format.
     *
     * @param reg IR register or input variable.
     * @return Assembly format of the register or input variable.
     */
    std::string IR_reg_to_asm(std::string reg);

    /**
     * @brief Generate the prologue of the function in assembly.
     *
     * @param o Output stream to write the assembly code.
     * @param target Target architecture.
     */
    void gen_asm_prologue(std::ostream& o, Target target);

    /**
     * @brief Generate the epilogue of the function in assembly.
     *
     * @param o Output stream to write the assembly code.
     * @param target Target architecture.
     */
    void gen_asm_epilogue(std::ostream& o, Target target);

    /**
     * @brief Add a variable to the symbol table.
     *
     * @param name Name of the variable.
     * @param t Information about the variable.
     */
    void add_to_symbol_table(std::string name, VariableInfo t);

    /**
     * @brief Get information about a variable from the symbol table.
     *
     * @param name Name of the variable.
     * @return Information about the variable.
     */
    VariableInfo get_var_info(std::string name);

    /**
     * @brief Generate a new basic block name.
     *
     * @return New basic block name.
     */
    std::string new_BB_name();

    BasicBlock* current_bb; /**< Pointer to the current basic block. */
    int memoryUse;          /**< Memory usage of the CFG. */

protected:
    std::unordered_map<std::string, VariableInfo>& variables; /**< Reference to the symbol table (variables). */
    std::string entry_block_label;                            /**< Label of the entry block. */
    int nextBBnumber;                                         /**< Just for naming. */

    std::vector<BasicBlock*> bbs; /**< All the basic blocks of this CFG. */
};

#endif
