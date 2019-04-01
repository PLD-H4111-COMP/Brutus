// ---------------------------------------------------------- C++ System Headers
#include <iostream>
#include <string>
#include <vector>

// ------------------------------------------------------------- Project Headers
#include "CProgAST.h"
#include "IR.h"

////////////////////////////////////////////////////////////////////////////////
// class CProgAST                                                             //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor

CProgASTProgram::~CProgASTProgram()
{
    for(CProgASTFuncdef* funcdef : funcdefs)
    {
        delete funcdef;
    }
}

// ------------------------------------------------------ Public Member Funcions
void CProgASTProgram::add_funcdef(CProgASTFuncdef* funcdef)
{
    funcdefs.push_back(funcdef);
}

std::vector<CFG*> CProgASTProgram::build_ir() const
{
    std::vector<CFG*> ir;
    for(CProgASTFuncdef* funcdef : funcdefs)
    {
        ir.push_back(funcdef->build_ir());
    }
    return ir;
}

////////////////////////////////////////////////////////////////////////////////
// class CProgASTFuncdef                                                      //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor
CProgASTFuncdef::CProgASTFuncdef(std::string id, Type type) :
    identifier(id), return_type(type)
{}

CProgASTFuncdef::~CProgASTFuncdef()
{
    for(CProgASTStatement* statement : statements)
    {
        delete statement;
    }
}

// ----------------------------------------------------- Public Member Functions
void CProgASTFuncdef::add_statement(CProgASTStatement* statement)
{
    statements.push_back(statement);
}

CFG* CProgASTFuncdef::build_ir() const
{
    CFG* cfg = new CFG(this);
    for(CProgASTStatement* statement : statements)
    {
        statement->build_ir(cfg);
    }
    return cfg;
}

////////////////////////////////////////////////////////////////////////////////
// class CProgASTReturn : public CProgASTStatement                            //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor
CProgASTReturn::CProgASTReturn(CProgASTExpression* expression) :
    return_expression(expression)
{}

CProgASTReturn::~CProgASTReturn()
{
    delete return_expression;
}

// ----------------------------------------------------- Public Member Functions
std::string CProgASTReturn::build_ir(CFG* cfg) const
{
    std::string rval = return_expression->build_ir(cfg);
    cfg->current_bb->add_IRInstr(IRInstr::ret, VarType(INT_64), {rval});
    return ""; // ??
}

////////////////////////////////////////////////////////////////////////////////
// class CProgASTDeclaration : public CProgASTStatement                       //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor
CProgASTDeclaration::CProgASTDeclaration(Type type) :
    type_specifier(type)
{}

CProgASTDeclaration::~CProgASTDeclaration()
{
    for(const CProgASTDeclarator* declarator : declarators)
    {
        delete declarator;
    }
}

// ----------------------------------------------------- Public Member Functions

void CProgASTDeclaration::add_declarator(CProgASTDeclarator* declarator)
{
    declarator->set_type(type_specifier);
    declarators.push_back(declarator);
}

std::string CProgASTDeclaration::build_ir(CFG* cfg) const
{
    for(const CProgASTDeclarator* declarator : declarators)
    {
        declarator->build_ir(cfg);
    }
    return ""; // ??
}

////////////////////////////////////////////////////////////////////////////////
// class CProgASTDeclarator                                                   //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor
CProgASTDeclarator::CProgASTDeclarator(CProgASTIdentifier* id, CProgASTAssignment* init) :
    identifier(id), initializer(init)
{}

CProgASTDeclarator::~CProgASTDeclarator()
{
    delete identifier;
    delete initializer;
}

// ----------------------------------------------------- Public Member Functions
void CProgASTDeclarator::set_type(Type type)
{
    type_specifier = type;
}

std::string CProgASTDeclarator::build_ir(CFG* cfg) const
{
    std::string name = identifier->getText();
    /*if(!cfg->tos.declared(name))
    {
        cfg->tos.add_declaration(name, type_specifier);
    }*/
    if(initializer != nullptr)
    {
        initializer->build_ir(cfg);
    }
    return ""; // ??
}

////////////////////////////////////////////////////////////////////////////////
// class CProgASTAssignment                                                   //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor
CProgASTAssignment::CProgASTAssignment(
        CProgASTIdentifier* identifier,
        CProgASTExpression* expression) :
    lhs_identifier(identifier),
    rhs_expression(expression)
{}

CProgASTAssignment::~CProgASTAssignment()
{
    delete lhs_identifier;
    delete rhs_expression;
}

// ----------------------------------------------------- Public Member Functions
std::string CProgASTAssignment::build_ir(CFG* cfg) const
{
    std::string name = lhs_identifier->getText();
    std::string init = rhs_expression->build_ir(cfg);
    //if(!cfg->tos.declared(name))
    {
        // error
    }
    cfg->current_bb->add_IRInstr(IRInstr::wmem, INT_64, {"%eax", init});
    cfg->current_bb->add_IRInstr(IRInstr::wmem, INT_64, {name, "%eax"});
}

////////////////////////////////////////////////////////////////////////////////
// class CProgASTAddition : public CProgASTExpression                         //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor
CProgASTAddition::CProgASTAddition(CProgASTExpression* lhs, CProgASTExpression* rhs) :
    lhs_operand(lhs), rhs_operand(rhs)
{}

CProgASTAddition::~CProgASTAddition()
{
    delete lhs_operand;
    delete rhs_operand;
}

// ----------------------------------------------------- Public Member Functions
std::string CProgASTAddition::build_ir(CFG* cfg) const
{
    std::string lhs_name = lhs_operand->build_ir(cfg);
    std::string rhs_name = rhs_operand->build_ir(cfg);
    std::string tmp_name = cfg->create_new_tempvar(INT_64);
    cfg->current_bb->add_IRInstr(IRInstr::add, INT_64, {tmp_name, lhs_name, rhs_name});
}

////////////////////////////////////////////////////////////////////////////////
// class CProgASTSubtraction : public CProgASTExpression                      //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor
CProgASTSubtraction::CProgASTSubtraction(CProgASTExpression* lhs, CProgASTExpression* rhs) :
    lhs_operand(lhs), rhs_operand(rhs)
{}

CProgASTSubtraction::~CProgASTSubtraction()
{
    delete lhs_operand;
    delete rhs_operand;
}

// ----------------------------------------------------- Public Member Functions
std::string CProgASTSubtraction::build_ir(CFG* cfg) const
{
    std::string lhs_name = lhs_operand->build_ir(cfg);
    std::string rhs_name = rhs_operand->build_ir(cfg);
    std::string tmp_name = cfg->create_new_tempvar(INT_64);
    cfg->current_bb->add_IRInstr(IRInstr::sub, INT_64, {tmp_name, lhs_name, rhs_name});
}

////////////////////////////////////////////////////////////////////////////////
// class CProgASTMultiplication : public CProgASTExpression                   //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor
CProgASTMultiplication::CProgASTMultiplication(CProgASTExpression* lhs, CProgASTExpression* rhs) :
    lhs_operand(lhs), rhs_operand(rhs)
{}

CProgASTMultiplication::~CProgASTMultiplication()
{
    delete lhs_operand;
    delete rhs_operand;
}

// ----------------------------------------------------- Public Member Functions
std::string CProgASTMultiplication::build_ir(CFG* cfg) const
{
    std::string lhs_name = lhs_operand->build_ir(cfg);
    std::string rhs_name = rhs_operand->build_ir(cfg);
    std::string tmp_name = cfg->create_new_tempvar(INT_64);
    cfg->current_bb->add_IRInstr(IRInstr::mul, INT_64, {tmp_name, lhs_name, rhs_name});
}

////////////////////////////////////////////////////////////////////////////////
// class CProgASTDivision : public CProgASTExpression                         //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor
CProgASTDivision::CProgASTDivision(CProgASTExpression* lhs, CProgASTExpression* rhs) :
    lhs_operand(lhs), rhs_operand(rhs)
{}

CProgASTDivision::~CProgASTDivision()
{
    delete lhs_operand;
    delete rhs_operand;
}

// ----------------------------------------------------- Public Member Functions
std::string CProgASTDivision::build_ir(CFG* cfg) const
{
    std::string lhs_name = lhs_operand->build_ir(cfg);
    std::string rhs_name = rhs_operand->build_ir(cfg);
    std::string tmp_name = cfg->create_new_tempvar(INT_64);
    cfg->current_bb->add_IRInstr(IRInstr::div, INT_64, {tmp_name, lhs_name, rhs_name});
}

////////////////////////////////////////////////////////////////////////////////
// class CProgASTModulo : public CProgASTExpression                           //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor
CProgASTModulo::CProgASTModulo(CProgASTExpression* lhs, CProgASTExpression* rhs) :
    lhs_operand(lhs), rhs_operand(rhs)
{}

CProgASTModulo::~CProgASTModulo()
{
    delete lhs_operand;
    delete rhs_operand;
}

// ----------------------------------------------------- Public Member Functions
std::string CProgASTModulo::build_ir(CFG* cfg) const
{
    std::string lhs_name = lhs_operand->build_ir(cfg);
    std::string rhs_name = rhs_operand->build_ir(cfg);
    std::string tmp_name = cfg->create_new_tempvar(INT_64);
    cfg->current_bb->add_IRInstr(IRInstr::mod, INT_64, {tmp_name, lhs_name, rhs_name});
}

////////////////////////////////////////////////////////////////////////////////
// class CProgASTUnaryMinus : public CProgASTExpression                       //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor
CProgASTUnaryMinus::CProgASTUnaryMinus(CProgASTExpression* expression) :
    inner_expression(expression)
{}

CProgASTUnaryMinus::~CProgASTUnaryMinus()
{
    delete inner_expression;
}

// ----------------------------------------------------- Public Member Functions
std::string CProgASTUnaryMinus::build_ir(CFG* cfg) const
{
    std::string exp_name = inner_expression->build_ir(cfg);
    std::string tmp_name = cfg->create_new_tempvar(INT_64);
    cfg->current_bb->add_IRInstr(IRInstr::neg, INT_64, {tmp_name, exp_name});
    return tmp_name;
}

////////////////////////////////////////////////////////////////////////////////
// class CProgASTIntLiteral : public CProgASTExpression                       //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor
CProgASTIntLiteral::CProgASTIntLiteral(int val)
    : value(val)
{}

// ----------------------------------------------------- Public Member Functions
std::string CProgASTIntLiteral::build_ir(CFG* cfg) const
{
    std::string tmp_name = cfg->create_new_tempvar(INT_64);
    std::string literal_str = std::to_string(value);
    cfg->current_bb->add_IRInstr(IRInstr::wmem, INT_64, {tmp_name, literal_str});
    return tmp_name;
}

////////////////////////////////////////////////////////////////////////////////
// class CProgASTIdentifier                                                   //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor
CProgASTIdentifier::CProgASTIdentifier(std::string identifier) :
    name(identifier)
{}

// ----------------------------------------------------- Public Member Functions
std::string CProgASTIdentifier::getText() const
{
    return name;
}

std::string CProgASTIdentifier::build_ir(CFG* cfg) const
{
    /*if(!cfg->tos.declared(name))
    {
        // error
    }
    else
    {
        cfg->tos[name].used = true;
    }*/
    return name;
}
