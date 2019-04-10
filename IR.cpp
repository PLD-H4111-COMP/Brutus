// ------------------------------------------------------------- Project Headers
#include "IR.h"
#include "Writer.h"

// ---------------------------------------------------------- C++ System Headers
#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

static std::vector<std::string> param_registers_64 = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

////////////////////////////////////////////////////////////////////////////////
// enum Type                                                                  //
////////////////////////////////////////////////////////////////////////////////

TypeProperties::TypeProperties(size_t size, const std::string &name) :
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
    { Type::CHAR,     TypeProperties(1, "char") },
    { Type::VOID,     TypeProperties(0, "void") }
};

////////////////////////////////////////////////////////////////////////////////
// class TableOfSymbols                                                       //
////////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------- Constructor
SymbolProperties::SymbolProperties(Type type, int index, bool initialized, bool used, bool callable, int arg_index) :
    type(type), index(index), initialized(initialized), used(used), callable(callable), arg_index(arg_index)
{}

// ----------------------------------------------------------------- Constructor
TableOfSymbols::TableOfSymbols(TableOfSymbols* parent) :
    parent(parent), size(0), next_arg_index(0), next_arg_offset(0), next_tmp_var_id(0)
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

void TableOfSymbols::add_arg(std::string identifier, Type type)
{
    if(next_arg_index < 6)
    {
        size += types.at(type).size;
        symbols[identifier] = SymbolProperties(type, get_next_free_symbol_index());
        symbols[identifier].arg_index = next_arg_index++;
    }
    else
    {
        symbols[identifier] = SymbolProperties(type, 16 + next_arg_offset);
        symbols[identifier].arg_index = next_arg_index++;
        next_arg_offset += types.at(type).size;
    }
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

const SymbolProperties& TableOfSymbols::get_arg(int index) const
{
    if(parent && index < parent->next_arg_index)
    {
        return parent->get_arg(index);
    }
    else if(index < next_arg_index)
    {
        auto it = std::find_if(symbols.begin(), symbols.end(),
            [index](const std::pair<std::string, SymbolProperties>& t) -> bool
            {
                return t.second.arg_index == index;
            }
        );
        return it->second;
    }
    throw std::out_of_range("TableOfSymbols::get_arg() : index is out of range");
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

int TableOfSymbols::get_nb_parameters() const
{
    return next_arg_index;
}

void TableOfSymbols::initialize(const std::string &identifier)
{
    symbols[identifier].initialized = true;
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
        case IRInstr::Operation::pre_pp:
            operation = "pre_pp";
        break;
        case IRInstr::Operation::pre_mm:
            operation = "pre_mm";
        break;
        case IRInstr::Operation::post_pp:
            operation = "post_pp";
        break;
        case IRInstr::Operation::post_mm:
            operation = "post_mm";
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
        case IRInstr::Operation::cmp_null:
            operation = "cmp_null";
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
        case IRInstr::Operation::land:
            operation = "and";
        break;
        case IRInstr::Operation::lor:
            operation = "or";
        break;
        case IRInstr::Operation::lnot:
            operation = "not";
        break;
        case IRInstr::Operation::ret:
            operation = "ret";
        break;
    }
    return os << operation;
}

IRInstr::IRInstr(BasicBlock* bb, Operation op, Type t, const std::vector<std::string> &params) :
    bb(bb), op(op), t(t), params(params)
{}

void IRInstr::gen_asm(Writer& w)
{
    int count_register;
    switch(op)
    {
        case Operation::ldconst:
            w.assembly(1) << x86_instr("mov", bb->cfg->get_var_type(params[0])) << " $" << params[1] << ", " << bb->cfg->IR_var_to_asm(params[0]) << std::endl;
        break;
        case Operation::add:
        {
            Type type = bb->cfg->get_max_type(params[1], params[2]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            w.assembly(1) << x86_mov_var_reg(params[2], "b", type) << std::endl;
            w.assembly(1) << x86_instr_reg_reg("add", type, "b", "a") << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (type < output_type)
                w.assembly(1) << x86_convert_reg_a(type, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::sub:
        {
            Type type = bb->cfg->get_max_type(params[1], params[2]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            w.assembly(1) << x86_mov_var_reg(params[2], "b", type) << std::endl;
            w.assembly(1) << x86_instr_reg_reg("sub", type, "b", "a") << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (type < output_type)
                w.assembly(1) << x86_convert_reg_a(type, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::mul:
        {
            Type type = bb->cfg->get_max_type(params[1], params[2]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            w.assembly(1) << x86_mov_var_reg(params[2], "b", type) << std::endl;
            w.assembly(1) << x86_instr_reg_reg("imul", type, "b", "a") << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (type < output_type)
                w.assembly(1) << x86_convert_reg_a(type, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::div:
        {
            Type type = bb->cfg->get_max_type(params[1], params[2]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            w.assembly(1) << x86_mov_var_reg(params[2], "b", type) << std::endl;
            w.assembly(1) << x86_extend_reg_a(type) << std::endl;
            w.assembly(1) << x86_instr_reg("idiv", type, "b") << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (type < output_type)
                w.assembly(1) << x86_convert_reg_a(type, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::mod:
        {
            Type type = bb->cfg->get_max_type(params[1], params[2]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            w.assembly(1) << x86_mov_var_reg(params[2], "b", type) << std::endl;
            w.assembly(1) << x86_extend_reg_a(type) << std::endl;
            w.assembly(1) << x86_instr_reg("idiv", type, "b") << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (type < output_type)
            {
                w.assembly(1) << x86_instr(x86_instr("movs", type) + "t", output_type) << " "
                              << IR_reg_to_asm("d", type) << ", " << IR_reg_to_asm("a", output_type) << std::endl;
            }
            else
                w.assembly(1) << x86_mov_reg_var("d", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::neg:
        {
            Type type = bb->cfg->get_var_type(params[1]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            w.assembly(1) << x86_instr_reg("neg", type, "a") << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (type < output_type)
                w.assembly(1) << x86_convert_reg_a(type, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::pre_pp:
             w.assembly(1) << x86_instr("inc", bb->cfg->get_var_type(params[0])) << " " << bb->cfg->IR_var_to_asm(params[0]) << std::endl;
        break;
        case Operation::pre_mm:
            w.assembly(1) << x86_instr("dec", bb->cfg->get_var_type(params[0])) << " " << bb->cfg->IR_var_to_asm(params[0]) << std::endl;
        break;
        case Operation::post_pp:

        break;
        case Operation::post_mm:

        break;
        case Operation::rmem:
        {
            Type type = bb->cfg->get_var_type(params[1]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (type < output_type)
                w.assembly(1) << x86_convert_reg_a(type, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::wmem:
        {
            Type type = bb->cfg->get_var_type(params[1]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (type < output_type)
                w.assembly(1) << x86_convert_reg_a(type, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::call:
            w.assembly(1) << "movq $0, %rax" << std::endl;
            count_register = 5;
            if (params.size() < 8)
            {
                count_register = params.size()-3;
            }
            for (size_t count_params = 2; count_params < params.size(); ++count_params) // passing parameters
            {
                if (count_params < 8)
                {
                    w.assembly(1) << x86_mov_var_reg(params[count_params], "a", Type::INT_64) << std::endl;
                    w.assembly(1) << "movq %rax, " << param_registers_64[count_register] << std::endl;
                }
                else
                {
                    w.assembly(1) << x86_mov_var_reg(params[count_params], "a", Type::INT_64) << std::endl;
                    w.assembly(1) << "pushq %rax" << std::endl;
                }
                --count_register;
            }
            w.assembly(1) << "call " << params[1] << std::endl;
            if (params[0] != "")
            {
                w.assembly(1) << x86_mov_reg_var("a", Type::INT_64, params[0]) << std::endl; // getting return value
            }
        break;
        case Operation::cmp_null:
            w.assembly(1) << x86_instr("cmp", bb->cfg->get_var_type(params[0])) << " $0, " << bb->cfg->IR_var_to_asm(params[0]) << std::endl;
        break;
        case Operation::cmp_eq:
        {
            Type type = bb->cfg->get_max_type(params[1], params[2]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            w.assembly(1) << x86_mov_var_reg(params[2], "b", type) << std::endl;
            w.assembly(1) << x86_instr_reg_reg("cmp", type, "b", "a") << std::endl;
            w.assembly(1) << "sete %al" << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (Type::CHAR < output_type)
                w.assembly(1) << x86_convert_reg_a(Type::CHAR, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::cmp_lt:
        {
            Type type = bb->cfg->get_max_type(params[1], params[2]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            w.assembly(1) << x86_mov_var_reg(params[2], "b", type) << std::endl;
            w.assembly(1) << x86_instr_reg_reg("cmp", type, "b", "a") << std::endl;
            w.assembly(1) << "setl %al" << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (Type::CHAR < output_type)
                w.assembly(1) << x86_convert_reg_a(Type::CHAR, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::cmp_le:
        {
            Type type = bb->cfg->get_max_type(params[1], params[2]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            w.assembly(1) << x86_mov_var_reg(params[2], "b", type) << std::endl;
            w.assembly(1) << x86_instr_reg_reg("cmp", type, "b", "a") << std::endl;
            w.assembly(1) << "setle %al" << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (Type::CHAR < output_type)
                w.assembly(1) << x86_convert_reg_a(Type::CHAR, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::cmp_gt:
        {
            Type type = bb->cfg->get_max_type(params[1], params[2]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            w.assembly(1) << x86_mov_var_reg(params[2], "b", type) << std::endl;
            w.assembly(1) << x86_instr_reg_reg("cmp", type, "b", "a") << std::endl;
            w.assembly(1) << "setg %al" << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (Type::CHAR < output_type)
                w.assembly(1) << x86_convert_reg_a(Type::CHAR, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::cmp_ge:
        {
            Type type = bb->cfg->get_max_type(params[1], params[2]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            w.assembly(1) << x86_mov_var_reg(params[2], "b", type) << std::endl;
            w.assembly(1) << x86_instr_reg_reg("cmp", type, "b", "a") << std::endl;
            w.assembly(1) << "setge %al" << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (Type::CHAR < output_type)
                w.assembly(1) << x86_convert_reg_a(Type::CHAR, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::cmp_ne:
        {
            Type type = bb->cfg->get_max_type(params[1], params[2]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            w.assembly(1) << x86_mov_var_reg(params[2], "b", type) << std::endl;
            w.assembly(1) << x86_instr_reg_reg("cmp", type, "b", "a") << std::endl;
            w.assembly(1) << "setne %al" << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (Type::CHAR < output_type)
                w.assembly(1) << x86_convert_reg_a(Type::CHAR, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::band:
        {
            Type type = bb->cfg->get_max_type(params[1], params[2]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            w.assembly(1) << x86_mov_var_reg(params[2], "b", type) << std::endl;
            w.assembly(1) << x86_instr_reg_reg("and", type, "b", "a") << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (type < output_type)
                w.assembly(1) << x86_convert_reg_a(type, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::bor:
        {
            Type type = bb->cfg->get_max_type(params[1], params[2]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            w.assembly(1) << x86_mov_var_reg(params[2], "b", type) << std::endl;
            w.assembly(1) << x86_instr_reg_reg("or", type, "b", "a") << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (type < output_type)
                w.assembly(1) << x86_convert_reg_a(type, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::bxor:
        {
            Type type = bb->cfg->get_max_type(params[1], params[2]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            w.assembly(1) << x86_mov_var_reg(params[2], "b", type) << std::endl;
            w.assembly(1) << x86_instr_reg_reg("xor", type, "b", "a") << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (type < output_type)
                w.assembly(1) << x86_convert_reg_a(type, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::bnot:
        {
            Type type = bb->cfg->get_var_type(params[1]);
            w.assembly(1) << x86_mov_var_reg(params[1], "a", type) << std::endl;
            w.assembly(1) << x86_instr_reg("not", type, "a") << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (type < output_type)
                w.assembly(1) << x86_convert_reg_a(type, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::land:

        break;
        case Operation::lor:

        break;
        case Operation::lnot:
        {
            w.assembly(1) << x86_instr("cmp", bb->cfg->get_var_type(params[1])) << " $0, " << bb->cfg->IR_var_to_asm(params[1]) << std::endl;
            w.assembly(1) << "sete %al" << std::endl;
            Type output_type = bb->cfg->get_var_type(params[0]);
            if (Type::CHAR < output_type)
                w.assembly(1) << x86_convert_reg_a(Type::CHAR, output_type) << std::endl;
            w.assembly(1) << x86_mov_reg_var("a", output_type, params[0]) << std::endl;
        }
        break;
        case Operation::ret:
            w.assembly(1) << x86_mov_var_reg(params[0], "a", Type::INT_64) << std::endl;
            w.assembly(1) << "jmp " << bb->cfg->get_last_bb()->label << std::endl;
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
        default:
            Writer::error() << "unexpected type " << types.at(type).name << " in IR_reg_to_asm" << std::endl;
            return "error";
    }
}

std::string IRInstr::x86_instr(const std::string &instr, Type type)
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
        default:
            Writer::error() << "unexpected type " << types.at(type).name << " in x86_instr" << std::endl;
            return "error";
    }
}

std::string IRInstr::x86_instr_reg(const std::string &instr, Type type, const std::string &reg) const
{
    return x86_instr(instr, type) + " " + IR_reg_to_asm(reg, type);
}

std::string IRInstr::x86_instr_reg_reg(const std::string &instr, Type type, const std::string &reg1, const std::string &reg2) const
{
    return x86_instr(instr, type) + " " + IR_reg_to_asm(reg1, type) + ", " + IR_reg_to_asm(reg2, type);
}

std::string IRInstr::x86_mov_var_reg(const std::string &var, const std::string &reg, Type reg_type, bool signed_fill) const
{
    Type var_type = bb->cfg->get_var_type(var);
    std::string instr;
    if (types.at(var_type).size >= types.at(reg_type).size)
        instr = x86_instr("mov", reg_type);
    else
        instr = x86_instr(x86_instr(signed_fill ? "movs" : "movz", var_type), reg_type);

    return instr + " " + bb->cfg->IR_var_to_asm(var) + ", " + IR_reg_to_asm(reg, reg_type);
}

std::string IRInstr::x86_mov_reg_var(const std::string &reg, Type reg_type, const std::string &var) const
{
    Type var_type = bb->cfg->get_var_type(var);
    std::string instr;
    if (types.at(reg_type).size >= types.at(var_type).size)
    {
        instr = x86_instr("mov", var_type);
        reg_type = var_type;
    }
    else
        Writer::error() << "reg_type < var_type in x86_mov_reg_var" << std::endl;

    return instr + " " + IR_reg_to_asm(reg, reg_type) + ", " + bb->cfg->IR_var_to_asm(var);
}

std::string IRInstr::x86_extend_reg_a(Type from)
{
    std::string to;
    switch (from)
    {
        case Type::CHAR:
            to = "w";
        break;
        case Type::INT_16:
            to = "d";
        break;
        case Type::INT_32:
            to = "d";
        break;
        case Type::INT_64:
            to = "o";
        break;
        default:
            to = "error";
            Writer::error() << "type error in x86_extend_reg_a" << std::endl;
        break;
    }
    return x86_instr("c", from) + "t" + to;
}

std::string IRInstr::x86_convert_reg_a(Type from, Type to)
{
    //return x86_instr(x86_instr("c", from) + "t", to);
    return x86_instr(x86_instr("movs", from), to) + " " + IR_reg_to_asm("a", from) + ", " + IR_reg_to_asm("a", to);
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


BasicBlock::BasicBlock(CFG* cfg, const std::string &entry_label) :
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
    writer.assembly(0) << label << ":" << std::endl;
    for (IRInstr* instr : instrs)
    {
        instr->gen_asm(writer);
    }

    if(instrs.empty())
    {
        if(exit_true)
        {
            writer.assembly(1) << "jmp " << exit_true->label << std::endl;
        }
        return;
    }

    if (instrs.back()->get_operation() == IRInstr::Operation::cmp_null)
    {
        writer.assembly(1) << "jne " << exit_true->label << std::endl;
        if(exit_false)
        {
            writer.assembly(1) << "jmp " << exit_false->label << std::endl;
        }
    }
    else if (instrs.back()->get_operation() == IRInstr::Operation::cmp_eq)
    {
        writer.assembly(1) << "je " << exit_true->label << std::endl;
        if(exit_false)
        {
            writer.assembly(1) << "jmp " << exit_false->label << std::endl;
        }
    }
    else if (instrs.back()->get_operation() == IRInstr::Operation::cmp_lt)
    {
        writer.assembly(1) << "jl " << exit_true->label << std::endl;
        if(exit_false)
        {
            writer.assembly(1) << "jmp " << exit_false->label << std::endl;
        }
    }
    else if (instrs.back()->get_operation() == IRInstr::Operation::cmp_le)
    {
        writer.assembly(1) << "jle " << exit_true->label << std::endl;
        if(exit_false)
        {
            writer.assembly(1) << "jmp " << exit_false->label << std::endl;
        }
    }
    else if (instrs.back()->get_operation() == IRInstr::Operation::cmp_gt)
    {
        writer.assembly(1) << "jg " << exit_true->label << std::endl;
        if(exit_false)
        {
            writer.assembly(1) << "jmp " << exit_false->label << std::endl;
        }
    }
    else if (instrs.back()->get_operation() == IRInstr::Operation::cmp_ge)
    {
        writer.assembly(1) << "jge " << exit_true->label << std::endl;
        if(exit_false)
        {
            writer.assembly(1) << "jmp " << exit_false->label << std::endl;
        }
    }
    else if (exit_false != nullptr)
    {
        std::string name = cfg->get_last_var_name();

        writer.assembly(1) << "movq " << cfg->get_var_index(name) << "(%rbp), %rax" << std::endl;
        writer.assembly(1) << "cmpq $0, %rax" << std::endl;
        writer.assembly(1) << "je " << exit_false->label << std::endl;
        writer.assembly(1) << "jne " << exit_true->label << std::endl;
    }
    else
    {
        if (!exit_true)
        {
            Writer::error() << instrs.size() << std::endl;
            Writer::error() << label << std::endl;
        }
        writer.assembly(1) << "jmp " << exit_true->label << std::endl;
    }
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

    if (symbols.get_nb_parameters() > 0)
    {
        int count_register = 5;
        if (symbols.get_nb_parameters() < 6)
        {
            count_register = symbols.get_nb_parameters()-1;
        }
        int limit = 6;
        if (symbols.get_nb_parameters() < 6)
        {
            limit = symbols.get_nb_parameters();
        }
        for (int count_param = 0; count_param < limit; ++count_param) {
            w.assembly(1) << "movq " << param_registers_64[count_register] << ", %rax" << std::endl;
            w.assembly(1) << IRInstr::x86_instr("mov", symbols.get_arg(count_param).type) << " " << IRInstr::IR_reg_to_asm("a", symbols.get_arg(count_param).type) << ", " << symbols.get_arg(count_param).index << "(%rbp)" << std::endl;
            --count_register;
        }
    }
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

bool CFG::is_initialized(const std::string &symbol_name) const
{
    return symbols.get_symbol(symbol_name).initialized;
}

void CFG::initialize(const std::string &symbol_name)
{
    symbols.initialize(symbol_name);
}

const SymbolProperties& CFG::get_symbol_properties(const std::string &symbol_name) const
{
    return symbols.get_symbol(symbol_name);
}

CFG::CFG(const CProgASTFuncdef* funcdef, const std::string &name, TableOfSymbols* global_symbols) :
    ast(funcdef), nextBBnumber(0), function_name(name), symbols(global_symbols)
{
    BasicBlock* entry = new BasicBlock(this, new_BB_name());
    BasicBlock* exit = new BasicBlock(this, new_BB_name());
    entry->exit_true = exit;
    bbs.push_back(entry);
    bbs.push_back(exit);
    current_bb = entry;
}


std::string CFG::new_BB_name()
{
    return "." + function_name + "_block" + std::to_string(nextBBnumber++);
}

BasicBlock* CFG::get_last_bb()
{
    return bbs.back();
}

void CFG::add_bb(BasicBlock* bb)
{
    bbs.insert(bbs.end()-1, bb);
}

void CFG::add_to_symbol_table(const std::string &name, Type type)
{
    symbols.add_symbol(name, type);
}

void CFG::add_arg_to_symbol_table(const std::string &name, Type type)
{
    symbols.add_arg(name, type);
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

std::string CFG::get_name()
{
    return function_name;
}

int CFG::get_nb_parameters() const
{
    return symbols.get_nb_parameters();
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
// class IR                                                                   //
////////////////////////////////////////////////////////////////////////////////

IR::IR(Writer &writer, const std::string &filename) : writer(writer), filename(filename)
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
    writer.assembly(1) << ".file\t\""+filename+"\"" << std::endl;
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
