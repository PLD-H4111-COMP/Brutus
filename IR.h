#pragma once

// ---------------------------------------------------------- C++ System Headers
#include <iostream>
#include <map>
#include <string>
#include <map>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations                                                       //
////////////////////////////////////////////////////////////////////////////////

class CProgASTFuncdef;
class BasicBlock;
class CFG;
class Writer;

////////////////////////////////////////////////////////////////////////////////
// enum Type                                                                  //
////////////////////////////////////////////////////////////////////////////////

enum class Type { INT_64, INT_32, INT_16, CHAR };

struct TypeProperties {
    // ------------------------------------------------------------- Constructor
    TypeProperties() = delete;
    TypeProperties(size_t size, std::string name);

    // ------------------------------------------------------- Public Properties
    const size_t size;
    const std::string name;

    // ---------------------------------------------------------- Static methods
    static Type max(Type a, Type b);
};

extern std::map<Type, const TypeProperties> types;

////////////////////////////////////////////////////////////////////////////////
// class TableOfSymbols                                                       //
////////////////////////////////////////////////////////////////////////////////

struct SymbolProperties {
    // ------------------------------------------------------------- Constructor
    SymbolProperties() = default;
    SymbolProperties(Type type, int index, bool initialized = false, bool used = false);

    // ------------------------------------------------------- Public Properties
    Type type;
    int index;
    bool initialized;
    bool used;
    // BasicBlock* scope;
};

class TableOfSymbols {
public:
    // ------------------------------------------------------------- Constructor
    TableOfSymbols();

    // ------------------------------------------------- Public Member Functions
    std::string add_tmp_var(Type type);
    void add_symbol(std::string identifier, Type type);
    bool is_declared(std::string identifier) const;
    const SymbolProperties& get_symbol(std::string identifier) const;
    size_t get_aligned_size(size_t alignment_size) const;
    const std::string get_last_symbol_name() const;

    void print_debug_infos() const;
protected:
    int get_next_free_symbol_index() const;

    std::map<std::string, SymbolProperties> symbols;
    size_t size;
    int next_tmp_var_id;
};

////////////////////////////////////////////////////////////////////////////////
// class IRInstr                                                              //
////////////////////////////////////////////////////////////////////////////////

//! The class for one 3-address instruction
class IRInstr {

public:
    /** The instructions themselves -- feel free to subclass instead */
    typedef enum {
        ldconst,
        add,
        sub,
        mul,
        div,
        mod,
        neg,
        rmem,
        wmem,
        call,
        cmp_eq,
        cmp_lt,
        cmp_le,
        ret
    } Operation;


    /**  constructor */
    IRInstr(BasicBlock* bb, Operation op, Type t, std::vector<std::string> params);

    /** Actual code generation */
    void gen_asm(Writer& writer); /**< x86 assembly code generation for this IR instruction */
    static std::string IR_reg_to_asm(const std::string &reg, Type type); /**< helper method: inputs a, IR reg , returns e.g. "eax" for for the Type::INT_32 reg "a" */

    void print_debug_infos() const;

    Operation get_operation() const;

private:
    std::string x86_instr(const std::string &instr, Type type) const;
    std::string x86_instr_var_reg(const std::string &instr, const std::string &var, const std::string &reg) const;
    std::string x86_instr_reg_var(const std::string &instr, const std::string &var, const std::string &reg) const;
    std::string x86_instr_reg(const std::string &instr, const std::string &reg, Type type) const;

    BasicBlock* bb; /**< The BB this instruction belongs to, which provides a pointer to the CFG this instruction belong to */
    Operation op;
    Type t;
    std::vector<std::string> params; /**< For 3-op instrs: d, x, y; for ldconst: d, c;  For call: label, d, params;  for wmem and rmem: choose yourself */
    // if you subclass IRInstr, each IRInstr subclass has its parameters and the previous (very important) comment becomes useless: it would be a better design.
};

std::ostream& operator<<(std::ostream& os, const IRInstr::Operation& op);

////////////////////////////////////////////////////////////////////////////////
// class BasicBlock                                                           //
////////////////////////////////////////////////////////////////////////////////

/**  The class for a basic block */

/* A few important comments.
     IRInstr has no jump instructions:
     assembly jumps are generated as follows in BasicBlock::gen_asm():
     1/ a cmp_* comparison instructions, if it is the last instruction of its block,
       generates an actual assembly comparison followed by a conditional jump to the exit_false branch
             If it is not the last instruction of its block, it behaves as an arithmetic two-operand instruction (add or mult)
         2/ BasicBlock::gen_asm() first calls IRInstr::gen_asm() on all its instructions, and then
            if  exit_true  is a  nullptr, it generates the epilogue
                if  exit_false is not a nullptr, and the last instruction is not a cmp, it generates two conditional branches based on the value of the last variable assigned
        otherwise it generates an unconditional jmp to the exit_true branch
*/

class BasicBlock {
public:
    BasicBlock(CFG* cfg, std::string entry_label);
    virtual ~BasicBlock();
    void gen_asm(Writer& writer); /**< x86 assembly code generation for this basic block (very simple) */

    void add_IRInstr(IRInstr::Operation op, Type t, std::vector<std::string> params);

    void print_debug_infos() const;

    // No encapsulation whatsoever here. Feel free to do better.
    BasicBlock* exit_true;  /**< pointer to the next basic block, true branch. If nullptr, return from procedure */
    BasicBlock* exit_false; /**< pointer to the next basic block, false branch. If null_ptr, the basic block ends with an unconditional jump */
    std::string label; /**< label of the BB, also will be the label in the generated code */
    CFG* cfg; /** < the CFG where this block belongs */
    std::vector<IRInstr*> instrs; /** < the instructions themselves. */
};

////////////////////////////////////////////////////////////////////////////////
// class CFG                                                                  //
////////////////////////////////////////////////////////////////////////////////

/** The class for the control flow graph, also includes the symbol table */

/* A few important comments:
     The entry block is the one with the same label as the AST function name.
       (it could be the first of bbs, or it could be defined by an attribute value)
     The exit block is the one with both exit pointers equal to nullptr.
     (again it could be identified in a more explicit way)

*/
class CFG {
public:
    CFG(const CProgASTFuncdef* funcdef, const std::string &name);

    void add_bb(BasicBlock* bb);

    // x86 code generation: could be encapsulated in a processor class in a retargetable compiler
    void gen_asm(Writer& writer);
    std::string IR_var_to_asm(const std::string &reg); /**< helper method: inputs a IR input variable, returns e.g. "-24(%rbp)" for the proper value of -24 */
    void gen_asm_prologue(Writer& writer);
    void gen_asm_epilogue(Writer& writer);

    // symbol table methods
    void add_to_symbol_table(const std::string &name, Type type);
    std::string create_new_tempvar(Type type);
    int get_var_index(const std::string &name) const;
    Type get_var_type(const std::string &name) const;
    bool is_declared(const std::string &name) const;
    Type get_max_type(const std::string &lhs, const std::string &rhs) const;
    std::string get_last_var_name() const;

    void print_debug_infos() const;
    void print_debug_infos_variables() const;

    const CProgASTFuncdef* ast; /**< The AST this CFG comes from */

    // basic block management
    std::string new_BB_name();
    BasicBlock* current_bb;

protected:
    TableOfSymbols symbols;
    int nextBBnumber; /**< just for naming */
    std::string function_name;

    std::vector <BasicBlock*> bbs; /**< all the basic blocks of this CFG*/
};

////////////////////////////////////////////////////////////////////////////////
// class IR                                                                   //
////////////////////////////////////////////////////////////////////////////////

class IR {
public :
    IR(Writer &writer);
    ~IR();
    void add_cfg(CFG* cfg);
    void gen_asm();
    void print_debug_infos() const;
private :
    Writer &writer;
    std::vector<CFG*> cfgs;
};
