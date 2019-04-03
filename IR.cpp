// ------------------------------------------------------------- Project Headers
#include "IR.h"
#include "Writer.h"

// ---------------------------------------------------------- C++ System Headers
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// enum Type                                                                  //
////////////////////////////////////////////////////////////////////////////////

TypeProperties::TypeProperties(size_t size, std::string name) :
    size(size), name(name)
{}

std::unordered_map<Type, const TypeProperties> types
{
    { INT_64,   TypeProperties(8, "int64_t") },
    { INT_32,   TypeProperties(4, "int32_t") },
    { INT_16,   TypeProperties(2, "int16_t") },
    { CHAR,     TypeProperties(1, "char") }
};

////////////////////////////////////////////////////////////////////////////////
// class TableOfSymbols                                                       //
////////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------- Constructor
SymbolProperties::SymbolProperties(Type type, int index, bool initialized, bool used) :
    type(type), index(index), initialized(initialized), used(used)
{}

// ----------------------------------------------------------------- Constructor
TableOfSymbols::TableOfSymbols() :
    next_free_symbol_index(0), next_tmp_var_id(0)
{}

// ----------------------------------------------------- Public Member Functions
std::string TableOfSymbols::add_tmp_var(Type type)
{
    std::string name = "!tmp" + std::to_string(next_tmp_var_id);
    symbols[name] = SymbolProperties(type, next_free_symbol_index);
    next_free_symbol_index -= types.at(type).size;
    next_tmp_var_id++;
    return name;
}

void TableOfSymbols::add_symbol(std::string identifier, Type type)
{
    symbols[identifier] = SymbolProperties(type, next_free_symbol_index);
    next_free_symbol_index -= types.at(type).size;
}

bool TableOfSymbols::is_declared(std::string identifier) const
{
    return symbols.find(identifier) != symbols.end();
}

const SymbolProperties& TableOfSymbols::get_symbol(std::string identifier) const
{
    return symbols.at(identifier);
}

void TableOfSymbols::print_debug_infos() const
{
    for(auto p : symbols)
    {
        std::clog << "Nom variable : " << p.first << ", Type : " << p.second.type << ", Index : " << p.second.index << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////
// class IRInstr                                                              //
////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const IRInstr::Operation& op)
{
    std::string operation;
    switch(op)
    {
        case IRInstr::Operation::ldconst:
            operation = "ldconst";
        break;
        case IRInstr::Operation::add:
            operation = "add";
        break;
        case IRInstr::Operation::sub:
            operation = "sub";
        break;
        case IRInstr::Operation::mul:
            operation = "mul";
        break;
        case IRInstr::Operation::div:
            operation = "div";
        break;
        case IRInstr::Operation::mod:
            operation = "mod";
        break;
        case IRInstr::Operation::neg:
            operation = "neg";
        break;
        case IRInstr::Operation::rmem:
            operation = "rmem";
        break;
        case IRInstr::Operation::wmem:
            operation = "wmem";
        break;
        case IRInstr::Operation::call:
            operation = "call";
        break;
        case IRInstr::Operation::cmp_eq:
            operation = "cmp_eq";
        break;
        case IRInstr::Operation::cmp_lt:
            operation = "cmp_lt";
        break;
        case IRInstr::Operation::cmp_le:
            operation = "cmp_le";
        break;
        case IRInstr::Operation::ret:
            operation = "ret";
        break;
    }
    return os << operation;
}

IRInstr::IRInstr(BasicBlock* bb, Operation op, Type t, std::vector<std::string> params) :
    bb(bb), op(op), t(t), params(params)
{}

void IRInstr::gen_asm(std::ostream& os)
{
    switch(op)
    {
        case Operation::ldconst:
            os << "movl $" << params[1] << ", " << bb->cfg->get_var_index(params[0]) << "%(rbp)" << std::endl;
        break;
        case Operation::add:
            os << "movl " << bb->cfg->get_var_index(params[1]) << "(%rbp), %eax" << std::endl;
            os << "addq " << bb->cfg->get_var_index(params[2]) << "(%rbp), %eax" << std::endl;
            os << "movl %eax, " << bb->cfg->get_var_index(params[0]) << "(%rbp)" << std::endl;
        break;
        case Operation::sub:
            os << "movl" << bb->cfg->get_var_index(params[1]) << "(%rbp), %eax" << std::endl;
            os << "subq " << bb->cfg->get_var_index(params[2]) << "(%rbp), %eax" << std::endl;
            os << "movl %eax, " << bb->cfg->get_var_index(params[0]) << "(%rbp)" << std::endl;
        break;
        case Operation::mul:
            os << "movl " << bb->cfg->get_var_index(params[1]) << "(%rbp)" << ", %eax" << std::endl;
            os << "imull " << bb->cfg->get_var_index(params[2]) << "(%rbp), %eax" << std::endl;
            os << "movl %eax, " << bb->cfg->get_var_index(params[0]) << "(%rbp)" << std::endl;
        break;
        case Operation::div:
            os << "movl " << bb->cfg->get_var_index(params[1]) << "(%rbp)" << ", %ebx" << std::endl;
            os << "movl " << bb->cfg->get_var_index(params[2]) << "(%rbp), %eax" << std::endl;
            os << "cltd" << std::endl;
            os << "idivl %ebx " << std::endl;
            os << "movl %eax, " << bb->cfg->get_var_index(params[0]) << "(%rbp)" << std::endl;
        break;
        case Operation::mod:
            os << "movl " << bb->cfg->get_var_index(params[1]) << "(%rbp)" << ", %ebx" << std::endl;
            os << "movl " << bb->cfg->get_var_index(params[2]) << "(%rbp), %eax" << std::endl;
            os << "cltd" << std::endl;
            os << "idivl %ebx" << std::endl;
            os << "movl %edx, " << bb->cfg->get_var_index(params[0]) << "(%rbp)" << std::endl;
        break;
        case Operation::neg:

        break;
        case Operation::rmem:
            os << "movl " << bb->cfg->get_var_index(params[1]) << "%(rbp), %eax" << std::endl;
            os << "movl " << "%eax, " << bb->cfg->get_var_index(params[0]) << "%(rbp)" << std::endl;
        break;
        case Operation::wmem:
            os << "movl " << bb->cfg->get_var_index(params[0]) << "(%rbp), %eax" << std::endl;
            os << "movl " << bb->cfg->get_var_index(params[1]) << "(%rbp), (%eax)" << std::endl;
        break;
        case Operation::call:

        break;
        case Operation::cmp_eq:

        break;
        case Operation::cmp_lt:

        break;
        case Operation::cmp_le:

        break;
        case Operation::ret:
            os << "movl " << bb->cfg->get_var_index(params[0]) << "(%rbp), %eax" << std::endl;
            os << "movl %rbp, %rsp" << std::endl;
            os << "pop %rbp" << std::endl;
            os << "ret" << std::endl;
        break;
    }
    os << std::endl;
}

void IRInstr::print()
{
    std::clog << "Type de retour : " << t << ", Operation : " << op << std::endl;
    std::clog << "Parametres : ";
    for (std::string param : params)
    {
        std::clog << param << ", ";
    }
    std::clog << std::endl;
}


////////////////////////////////////////////////////////////////////////////////
// class BasicBlock                                                           //
////////////////////////////////////////////////////////////////////////////////


BasicBlock::BasicBlock(CFG* cfg, std::string entry_label) :
    exit_true(nullptr), exit_false(nullptr), label(entry_label)
{
    // jump to the next block default ?
    this->cfg = cfg;
}

BasicBlock::~BasicBlock()
{
    for (IRInstr* instr : instrs)
    {
        delete instr;
    }
}

void BasicBlock::gen_asm(std::ostream &o)
{
    for (IRInstr* instr : instrs){
        instr->gen_asm(o);
    }
}

void BasicBlock::add_IRInstr(IRInstr::Operation op, Type t, std::vector<std::string> params)
{
    instrs.push_back(new IRInstr(this, op, t, params));
}

void BasicBlock::print()
{
    std::clog << "Basic Bloc : " << label << std::endl;
    // Amelioration : ajouter les noms des blocs suivants (exit_true, exit_false)
    for (IRInstr* instr : instrs)
    {
        instr->print();
    }
}

////////////////////////////////////////////////////////////////////////////////
// class CFG                                                                  //
////////////////////////////////////////////////////////////////////////////////

void CFG::gen_asm(std::ostream& o)
{
    for (BasicBlock* bb : bbs){
        bb->gen_asm(o);
    }
}

std::string CFG::IR_reg_to_asm(std::string reg)
{
    return "";
}

void CFG::gen_asm_prologue(std::ostream& o)
{

}

void CFG::gen_asm_epilogue(std::ostream& o)
{

}


int CFG::get_var_index(std::string name)
{
    return symbols.get_symbol(name).index;
}

Type CFG::get_var_type(std::string name)
{
    return symbols.get_symbol(name).type;
}

CFG::CFG(const CProgASTFuncdef* fundcef) :
    ast(fundcef)
{
    bbs.push_back(new BasicBlock(this, "input"));
    current_bb = new BasicBlock(this, "first_bb");
    bbs.push_back(current_bb);
    bbs.push_back(new BasicBlock(this, "output"));
}


std::string CFG::new_BB_name()
{
    return "block_" + std::to_string(nextBBnumber++);
}


void CFG::add_bb(BasicBlock* bb)
{
    bbs.insert(bbs.end()-1, bb);
}

void CFG::add_to_symbol_table(std::string name, Type type)
{
    symbols.add_symbol(name, type);
}

std::string CFG::create_new_tempvar(Type type)
{
    return symbols.add_tmp_var(type);
}

void CFG::print()
{
    for (BasicBlock* bb : bbs)
    {
        bb->print();
    }
}

void CFG::printVariables()
{
    symbols.print_debug_infos();
}

////////////////////////////////////////////////////////////////////////////////
// class IRStore                                                              //
////////////////////////////////////////////////////////////////////////////////

IRStore::IRStore() {

}

void IRStore::add_cfg(CFG* cfg)
{
	cfgs.push_back(cfg);
}

IRStore::~IRStore()
{
    for (CFG* cfg : cfgs)
    {
        delete cfg;
    }
}

void IRStore::print_IR()
{
    int i = 0;
    std::clog << "Affichage de l'IR : " << std::endl;
    for (CFG* cfg : cfgs)
    {
        std::clog << "CFG " << i << " : " << std::endl;
        cfg->print();
        ++i;
    }
}

void IRStore::gen_asm(std::ostream& o){
    for (CFG* cfg : cfgs){
        cfg->gen_asm(o);
    }
}
