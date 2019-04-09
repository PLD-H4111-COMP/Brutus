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

Type TypeProperties::max(Type a, Type b)
{
    return types.at(a).size >= types.at(b).size ? a : b;
}

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
SymbolProperties::SymbolProperties(Type type, int index, bool initialized, bool used, bool callable) :
    type(type), index(index), initialized(initialized), used(used), callable(callable)
{}

// ----------------------------------------------------------------- Constructor
TableOfSymbols::TableOfSymbols(TableOfSymbols* parent) :
    parent(parent), size(0), next_tmp_var_id(0)
{}

// ----------------------------------------------------- Public Member Functions
std::string TableOfSymbols::add_tmp_var(Type type)
{
    std::string name = "!tmp" + std::to_string(next_tmp_var_id);
    next_tmp_var_id++;
    size += types.at(type).size;
    symbols[name] = SymbolProperties(type, get_next_free_symbol_index());
    return name;
}

void TableOfSymbols::add_symbol(std::string identifier, Type type)
{
    size += types.at(type).size;
    symbols[identifier] = SymbolProperties(type, get_next_free_symbol_index());
}

bool TableOfSymbols::is_declared(std::string identifier) const
{
    if(parent && parent->symbols.find(identifier) != parent->symbols.end())
    {
        return true;
    }
    return symbols.find(identifier) != symbols.end();
}

const SymbolProperties& TableOfSymbols::get_symbol(std::string identifier) const
{
    if(parent && parent->symbols.find(identifier) != parent->symbols.end())
    {
        return parent->symbols.at(identifier);
    }
    return symbols.at(identifier);
}

SymbolProperties& TableOfSymbols::get_symbol(std::string identifier)
{
    if(parent && parent->symbols.find(identifier) != parent->symbols.end())
    {
        return parent->symbols.at(identifier);
    }
    return symbols.at(identifier);
}

const std::string TableOfSymbols::get_last_symbol_name() const
{
    std::string name = "!tmp" + std::to_string(next_tmp_var_id-1);
    return name;
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
        Writer::info() << "Nom variable : " << p.first << ", Type : " << types.at(p.second.type).name << ", Index : " << p.second.index << std::endl;
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
        case IRInstr::Operation::cmp_gt:
            operation = "cmp_gt";
        break;
        case IRInstr::Operation::cmp_ge:
            operation = "cmp_ge";
        break;
        case IRInstr::Operation::cmp_ne:
            operation = "cmp_ne";
        break;
        case IRInstr::Operation::band:
            operation = "band";
        break;
        case IRInstr::Operation::bor:
            operation = "bor";
        break;
        case IRInstr::Operation::bxor:
            operation = "bxor";
        break;
        case IRInstr::Operation::bnot:
            operation = "bnot";
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

void IRInstr::gen_asm(Writer& w)
{
    switch(op)
    {
        case Operation::ldconst:
            w.assembly(1) << x86_instr("mov", bb->cfg->get_var_type(params[0])) << " $" << params[1] << ", " << bb->cfg->IR_var_to_asm(params[0]) << std::endl;
        break;
        case Operation::add:
            w.assembly(1) << x86_instr_var_reg("mov", params[1], "a") << std::endl;
            w.assembly(1) << x86_instr_var_reg("add", params[2], "a") << std::endl;
            w.assembly(1) << x86_instr_reg_var("mov", "a", params[0]) << std::endl;
        break;
        case Operation::sub:
            w.assembly(1) << x86_instr_var_reg("mov", params[1], "a") << std::endl;
            w.assembly(1) << x86_instr_var_reg("sub", params[2], "a") << std::endl;
            w.assembly(1) << x86_instr_reg_var("mov", "a", params[0]) << std::endl;
        break;
        case Operation::mul:
            w.assembly(1) << x86_instr_var_reg("mov", params[1], "a") << std::endl;
            w.assembly(1) << x86_instr_var_reg("imul", params[2], "a") << std::endl;
            w.assembly(1) << x86_instr_reg_var("mov", "a", params[0]) << std::endl;
        break;
        case Operation::div:
            w.assembly(1) << x86_instr_var_reg("mov", params[1], "a") << std::endl;
            w.assembly(1) << x86_instr_var_reg("mov", params[2], "b") << std::endl;
            w.assembly(1) << "cqto" << std::endl; // TODO : fix
            w.assembly(1) << x86_instr_reg("idiv", "b", bb->cfg->get_var_type(params[0])) << std::endl; // TODO : check type
            w.assembly(1) << x86_instr_reg_var("mov", "a", params[0]) << std::endl;
        break;
        case Operation::mod:
            w.assembly(1) << x86_instr_var_reg("mov", params[1], "a") << std::endl;
            w.assembly(1) << x86_instr_var_reg("mov", params[2], "b") << std::endl;
            w.assembly(1) << "cqto" << std::endl; // TODO : fix
            w.assembly(1) << x86_instr_reg("idiv", "b", bb->cfg->get_var_type(params[0])) << std::endl; // TODO : check type
            w.assembly(1) << x86_instr_reg_var("mov", "d", params[0]) << std::endl;
        break;
        case Operation::neg:
            w.assembly(1) << x86_instr_var_reg("mov", params[1], "a") << std::endl;
            w.assembly(1) << x86_instr_reg("neg", "a", bb->cfg->get_var_type(params[1])) << std::endl;
            w.assembly(1) << x86_instr_reg_var("mov", "a", params[0]) << std::endl;
        break;
        case Operation::rmem:
            w.assembly(1) << x86_instr_var_reg("mov", params[1], "a") << std::endl;
            w.assembly(1) << x86_instr_reg_var("mov", "a", params[0]) << std::endl;
        break;
        case Operation::wmem:
            w.assembly(1) << x86_instr_var_reg("mov", params[1], "a") << std::endl;
            w.assembly(1) << x86_instr_reg_var("mov", "a", params[0]) << std::endl;
        break;
        case Operation::call:
            w.assembly(1) << "movq $0, %rax" << std::endl;
            w.assembly(1) << "call " << params[1] << std::endl;
        break;
        case Operation::cmp_eq:
            w.assembly(1) << x86_instr_var_reg("mov", params[1], "a") << std::endl;
            w.assembly(1) << x86_instr_reg_var("cmp", "a", params[2]) << std::endl;
            w.assembly(1) << "sete %al" << std::endl;
            w.assembly(1) << "movzbq %al, %rax" << std::endl;
            w.assembly(1) << x86_instr_reg_var("mov", "a", params[0]) << std::endl;
        break;
        case Operation::cmp_lt:

        break;
        case Operation::cmp_le:

        break;
        case Operation::cmp_gt:

        break;
        case Operation::cmp_ge:

        break;
        case Operation::cmp_ne:

        break;
        case Operation::band:
            w.assembly(1) << x86_instr_var_reg("mov", params[1], "d") << std::endl;
            w.assembly(1) << x86_instr_var_reg("and", params[2], "d") << std::endl;
            w.assembly(1) << x86_instr_reg_var("mov", "d", params[0]) << std::endl;
        break;
        case Operation::bor:
            w.assembly(1) << x86_instr_var_reg("mov", params[1], "d") << std::endl;
            w.assembly(1) << x86_instr_var_reg("or", params[2], "d") << std::endl;
            w.assembly(1) << x86_instr_reg_var("mov", "d", params[0]) << std::endl;
        break;
        case Operation::bxor:
            w.assembly(1) << x86_instr_var_reg("mov", params[1], "d") << std::endl;
            w.assembly(1) << x86_instr_var_reg("xor", params[2], "d") << std::endl;
            w.assembly(1) << x86_instr_reg_var("mov", "d", params[0]) << std::endl;
        break;
        case Operation::bnot:
            w.assembly(1) << x86_instr_var_reg("mov", params[1], "a") << std::endl;
            w.assembly(1) << x86_instr_reg("not", "a", bb->cfg->get_var_type(params[1])) << std::endl;
            w.assembly(1) << x86_instr_reg_var("mov", "a", params[0]) << std::endl;
        break;
        case Operation::ret:
            w.assembly(1) << x86_instr_var_reg("mov", params[0], "a") << std::endl;
        break;
    }
}

std::string IRInstr::IR_reg_to_asm(const std::string &reg, Type type)
{
    switch (type)
    {
        case Type::CHAR:
            return "%" + reg + "l";
        case Type::INT_16:
            return "%" + reg + "x";
        case Type::INT_32:
            return "%e" + reg + "x";
        case Type::INT_64:
            return "%r" + reg + "x";
    }

    return "error";
}

std::string IRInstr::x86_instr(const std::string &instr, Type type) const
{
    switch (type)
    {
        case Type::CHAR:
            return instr + "b";
        case Type::INT_16:
            return instr + "w";
        case Type::INT_32:
            return instr + "l";
        case Type::INT_64:
            return instr + "q";
    }
    return "error type";
}

std::string IRInstr::x86_instr_var_reg(const std::string &instr, const std::string &var, const std::string &reg) const
{
    Type type = bb->cfg->get_var_type(var);
    return x86_instr(instr, type) + " " + bb->cfg->IR_var_to_asm(var) + ", " + IR_reg_to_asm(reg, type);
}

std::string IRInstr::x86_instr_reg_var(const std::string &instr, const std::string &reg, const std::string &var) const
{
    Type type = bb->cfg->get_var_type(var);
    return x86_instr(instr, type) + " " + IR_reg_to_asm(reg, type) + ", " + bb->cfg->IR_var_to_asm(var);
}


std::string IRInstr::x86_instr_reg(const std::string &instr, const std::string &reg, Type type) const
{
    return x86_instr(instr, type) + " " + IR_reg_to_asm(reg, type);
}


void IRInstr::print_debug_infos() const
{
    Writer::info() << "Type de retour : " << types.at(t).name << ", Operation : " << op << std::endl;
    Writer::info() << "Parametres : ";
    for (std::string param : params)
    {
        Writer::info() << param << ", ";
    }
    Writer::info() << std::endl;
}

IRInstr::Operation IRInstr::get_operation() const
{
    return op;
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

void BasicBlock::gen_asm(Writer& writer)
{
    writer.assembly(0) << "." << label << ":" << std::endl;
    for (IRInstr* instr : instrs){
        instr->gen_asm(writer);
    }
    
    /*if (instrs.back()->get_operation() == IRInstr::Operation::cmp_eq)
    {
        writer.assembly(1) << "jne " << exit_false->label << std::endl;
    }
    else if (exit_false != nullptr)
    {
        std::string name = cfg->get_last_var_name();
        
        writer.assembly(1) << "movq " << cfg->get_var_index(name) << "(%rbp), %rax" << std::endl;
        writer.assembly(1) << "cmp %rax, $0" << std::endl;
        writer.assembly(1) << "je " << exit_false->label << std::endl;
    }
    else
    {
        writer.assembly(1) << "jmp " << exit_true->label << std::endl;
    }*/
}

void BasicBlock::add_IRInstr(IRInstr::Operation op, Type t, std::vector<std::string> params)
{
    instrs.push_back(new IRInstr(this, op, t, params));
}

void BasicBlock::print_debug_infos() const
{
    Writer::info() << "Basic Bloc : " << label << std::endl;
    // Amelioration : ajouter les noms des blocs suivants (exit_true, exit_false)
    for (IRInstr* instr : instrs)
    {
        instr->print_debug_infos();
    }
}

////////////////////////////////////////////////////////////////////////////////
// class CFG                                                                  //
////////////////////////////////////////////////////////////////////////////////

void CFG::gen_asm(Writer& writer)
{
    for (BasicBlock* bb : bbs){
        bb->gen_asm(writer);
    }
}

std::string CFG::IR_var_to_asm(const std::string &var)
{
    return std::to_string(get_var_index(var)) + "(%rbp)";
}

void CFG::gen_asm_prologue(Writer& w){
    w.assembly(1) << ".globl\t" << function_name << std::endl;
    //w.assembly(1) << ".type\t" << function_name << ", @function" << std::endl;
    w.assembly(0) << function_name << ":" << std::endl;
    w.assembly(1) << "pushq %rbp" << std::endl;
    w.assembly(1) << "movq %rsp, %rbp" << std::endl;
    size_t stack_size = symbols.get_aligned_size(32);
    if (stack_size != 0)
        w.assembly(1) << "subq $" << std::to_string(stack_size) << ", %rsp" << std::endl;
}

void CFG::gen_asm_epilogue(Writer& w){
    w.assembly(1) << "movq %rbp, %rsp" << std::endl;
    w.assembly(1) << "popq %rbp" << std::endl;
    w.assembly(1) << "ret" << std::endl;
}


int CFG::get_var_index(const std::string &name) const
{
    if(symbols.is_declared(name))
    {
        return symbols.get_symbol(name).index;
    }
    Writer::error() << "use of undeclared identifier '" << name << "'" << std::endl;
    return 0;
}

std::string CFG::get_last_var_name() const
{
    return symbols.get_last_symbol_name();
}

Type CFG::get_var_type(const std::string &name) const
{
    if(symbols.is_declared(name))
    {
        return symbols.get_symbol(name).type;
    }
    Writer::error() << "use of undeclared identifier '" << name << "'" << std::endl;
    return Type::INT_64;
}

CFG::CFG(const CProgASTFuncdef* fundcef, const std::string &name, TableOfSymbols* global_symbols) :
    ast(fundcef), function_name(name), symbols(global_symbols)
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

void CFG::add_to_symbol_table(const std::string &name, Type type)
{
    symbols.add_symbol(name, type);
}

std::string CFG::create_new_tempvar(Type type)
{
    return symbols.add_tmp_var(type);
}

bool CFG::is_declared(const std::string &name) const
{
    return symbols.is_declared(name);
}

Type CFG::get_max_type(const std::string &lhs, const std::string &rhs) const
{
    return TypeProperties::max(get_var_type(lhs), get_var_type(rhs));
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

IR::IR(Writer &writer) : writer(writer)
{

}

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

void IR::gen_asm(){
    writer.assembly(1) << ".file\t\"ret42.c\"" << std::endl;
    writer.assembly(1) << ".text" << std::endl;
    for (CFG* cfg : cfgs){
        cfg->gen_asm_prologue(writer);
        cfg->gen_asm(writer);
        cfg->gen_asm_epilogue(writer);
    }
}

void IR::print_debug_infos() const
{
    Writer::info() << "Affichage de l'IR : " << std::endl;
    for (size_t i=0; i<cfgs.size(); ++i)
    {
        Writer::info() << "CFG " << i << " : " << std::endl;
        cfgs[i]->print_debug_infos();
    }
}

