// ------------------------------------------------------------- Project Headers
#include "IR.h"
#include "Writer.h"

// ---------------------------------------------------------- C++ System Headers
#include <iostream>
#include <string>
#include <map>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// enum Type                                                                  //
////////////////////////////////////////////////////////////////////////////////

TypeProperties::TypeProperties(size_t size, std::string name) :
    size(size), name(name)
{}

std::map<Type, const TypeProperties> types =
{
    { Type::INT_64,   TypeProperties(8, "int64_t") },
    { Type::INT_32,   TypeProperties(4, "int32_t") },
    { Type::INT_16,   TypeProperties(2, "int16_t") },
    { Type::CHAR,     TypeProperties(1, "char") }
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
    size(0), next_tmp_var_id(0)
{}

// ----------------------------------------------------- Public Member Functions
std::string TableOfSymbols::add_tmp_var(Type type)
{
    std::string name = "!tmp" + std::to_string(next_tmp_var_id);
    symbols[name] = SymbolProperties(type, get_next_free_symbol_index());
    size += types.at(type).size;
    next_tmp_var_id++;
    return name;
}

void TableOfSymbols::add_symbol(std::string identifier, Type type)
{
    symbols[identifier] = SymbolProperties(type, get_next_free_symbol_index());
    size += types.at(type).size;
}

bool TableOfSymbols::is_declared(std::string identifier) const
{
    return symbols.find(identifier) != symbols.end();
}

const SymbolProperties& TableOfSymbols::get_symbol(std::string identifier) const
{
    return symbols.at(identifier);
}

size_t TableOfSymbols::get_aligned_size(size_t alignment_size) const
{
    const size_t remainder = size % alignment_size;
    if (!remainder)
        return size;
    else
        return size + (alignment_size-remainder);
}

int TableOfSymbols::get_next_free_symbol_index() const
{
    return -static_cast<int>(size);
}

void TableOfSymbols::print_debug_infos() const
{
    for(auto p : symbols)
    {
        std::clog << "Nom variable : " << p.first << ", Type : " << types.at(p.second.type).name << ", Index : " << p.second.index << std::endl;
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
            os << "movq $" << params[1] << ", " << bb->cfg->get_var_index(params[0]) << "(%rbp)" << std::endl;
        break;
        case Operation::add:
            os << "movq " << bb->cfg->get_var_index(params[1]) << "(%rbp), %rax" << std::endl;
            os << "addq " << bb->cfg->get_var_index(params[2]) << "(%rbp), %rax" << std::endl;
            os << "movq %rax, " << bb->cfg->get_var_index(params[0]) << "(%rbp)" << std::endl;
        break;
        case Operation::sub:
            os << "movq " << bb->cfg->get_var_index(params[1]) << "(%rbp), %rax" << std::endl;
            os << "subq " << bb->cfg->get_var_index(params[2]) << "(%rbp), %rax" << std::endl;
            os << "movq %rax, " << bb->cfg->get_var_index(params[0]) << "(%rbp)" << std::endl;
        break;
        case Operation::mul:
            os << "movq " << bb->cfg->get_var_index(params[1]) << "(%rbp)" << ", %rax" << std::endl;
            os << "imulq " << bb->cfg->get_var_index(params[2]) << "(%rbp), %rax" << std::endl;
            os << "movq %rax, " << bb->cfg->get_var_index(params[0]) << "(%rbp)" << std::endl;
        break;
        case Operation::div:
            os << "movq " << bb->cfg->get_var_index(params[1]) << "(%rbp), %rax" << std::endl;
            os << "movq " << bb->cfg->get_var_index(params[2]) << "(%rbp)" << ", %rbx" << std::endl;
            os << "cqto" << std::endl;
            os << "idivq %rbx " << std::endl;
            os << "movq %rax, " << bb->cfg->get_var_index(params[0]) << "(%rbp)" << std::endl;
        break;
        case Operation::mod:
            os << "movq " << bb->cfg->get_var_index(params[1]) << "(%rbp), %rax" << std::endl;
            os << "movq " << bb->cfg->get_var_index(params[2]) << "(%rbp)" << ", %rbx" << std::endl;
            os << "cqto" << std::endl;
            os << "idivq %rbx" << std::endl;
            os << "movq %rdx, " << bb->cfg->get_var_index(params[0]) << "(%rbp)" << std::endl;
        break;
        case Operation::neg:
            os << "movq " << bb->cfg->get_var_index(params[1]) << "(%rbp), %rax" << std::endl;
            os << "negq %rax" << std::endl;
            os << "movq %rax, " << bb->cfg->get_var_index(params[0]) << "(%rbp)" << std::endl;
        break;
        case Operation::rmem:
            os << "movq " << bb->cfg->get_var_index(params[1]) << "%(rbp), %rax" << std::endl;
            os << "movq " << "%rax, " << bb->cfg->get_var_index(params[0]) << "%(rbp)" << std::endl;
        break;
        case Operation::wmem:
            os << "movq " << bb->cfg->get_var_index(params[1]) << "(%rbp), %rax" << std::endl;
            os << "movq %rax, " << bb->cfg->get_var_index(params[0]) << "(%rbp)" << std::endl;
        break;
        case Operation::call:
            os << "movq $0, %rax" << std::endl;
            os << "call " << params[1] << std::endl;
        break;
        case Operation::cmp_eq:

        break;
        case Operation::cmp_lt:

        break;
        case Operation::cmp_le:

        break;
        case Operation::ret:
            os << "movq " << bb->cfg->get_var_index(params[0]) << "(%rbp), %rax" << std::endl;
        break;
    }
    os << std::endl;
}

void IRInstr::print_debug_infos() const
{
    std::clog << "Type de retour : " << types.at(t).name << ", Operation : " << op << std::endl;
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

void BasicBlock::print_debug_infos() const
{
    std::clog << "Basic Bloc : " << label << std::endl;
    // Amelioration : ajouter les noms des blocs suivants (exit_true, exit_false)
    for (IRInstr* instr : instrs)
    {
        instr->print_debug_infos();
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

void CFG::gen_asm_prologue(std::ostream& os){
    os << "\t.globl\t" << function_name << std::endl;
    os << "\t.type\t" << function_name << ", @function" << std::endl;
    os << function_name << ":" << std::endl;
    os << "pushq %rbp" << std::endl;
    os << "movq %rsp, %rbp" << std::endl;
    size_t stack_size = symbols.get_aligned_size(32);
    if (stack_size != 0)
        os << "subq $" << std::to_string(stack_size) << ", %rsp" << std::endl;
}

void CFG::gen_asm_epilogue(std::ostream& os){
    os << "movq %rbp, %rsp" << std::endl;
    os << "pop %rbp" << std::endl;
    os << "ret" << std::endl;
}


int CFG::get_var_index(std::string name)
{
    if(symbols.is_declared(name))
    {
        return symbols.get_symbol(name).index;
    }
    std::cerr << "error: use of undeclared identifier '" << name << "'" << std::endl;
    return 0;
}

Type CFG::get_var_type(std::string name)
{
    if(symbols.is_declared(name))
    {
        return symbols.get_symbol(name).type;
    }
    std::cerr << "error: use of undeclared identifier '" << name << "'" << std::endl;
    return Type::INT_64;
}

CFG::CFG(const CProgASTFuncdef* fundcef, std::string name) :
    ast(fundcef), function_name(name)
{
    current_bb = new BasicBlock(this, "entry");
    bbs.push_back(current_bb);
    bbs.push_back(new BasicBlock(this, "exit"));
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

bool CFG::is_declared(std::string name) const
{
    return symbols.is_declared(name);
}

void CFG::print_debug_infos() const
{
    for (BasicBlock* bb : bbs)
    {
        bb->print_debug_infos();
    }
}

void CFG::print_debug_infos_variables() const
{
    symbols.print_debug_infos();
}

////////////////////////////////////////////////////////////////////////////////
// class IRStore                                                              //
////////////////////////////////////////////////////////////////////////////////

IR::~IR()
{
    for (CFG* cfg : cfgs)
    {
        delete cfg;
    }
}

void IR::add_cfg(CFG* cfg)
{
	cfgs.push_back(cfg);
}

void IR::gen_asm(std::ostream& os){
    os << "\t.file\t\"ret42.c\"" << std::endl;
    os << "\t.text" << std::endl;
    for (CFG* cfg : cfgs){
        cfg->gen_asm_prologue(os);
        cfg->gen_asm(os);
        cfg->gen_asm_epilogue(os);
    }
}

void IR::print_debug_infos() const
{
    std::clog << "Affichage de l'IR : " << std::endl;
    for (size_t i=0; i<cfgs.size(); ++i)
    {
        std::clog << "CFG " << i << " : " << std::endl;
        cfgs[i]->print_debug_infos();
    }
}

