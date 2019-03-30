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

CProgAST::~CProgAST()
{
    for(CProgASTFuncdef* funcdef : funcdef_nodes)
    {
        delete funcdef;
    }
}

// ------------------------------------------------------ Public Member Funcions
void CProgAST::add_funcdef(CProgASTFuncdef* funcdef)
{
    funcdef_nodes.push_back(funcdef);
}

std::vector<CFG*> build_ir() const
{
    std::vector<CFG*> ir;
    for(CProgASTFuncdef* funcdef : funcdef_nodes)
    {
        ir.push_back(funcdef->build_ir());
    }
    return ir;
}

////////////////////////////////////////////////////////////////////////////////
// class CProgASTFuncdef                                                      //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor
CProgASTFuncdef::CProgASTFuncdef(std::string identifier, Type type) :
    function_identifier(identifier), return_type(type)
{}

CProgASTFuncdef::~CProgASTFuncdef()
{
    for(CProgASTStatement* statement : statement_nodes)
    {
        delete statement;
    }
}

// ----------------------------------------------------- Public Member Functions
void CProgASTFuncdef::add_statement(CProgASTStatement* statement)
{
    statement_nodes.push_back(statement);
}

CFG* CProgASTFuncdef::build_ir() const
{
    CFG* cfg = new CFG(this);
    for(CProgASTStatement* statement : statement_nodes)
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
    delete expression;
}

// ----------------------------------------------------- Public Member Functions
std:string CProgASTReturn::build_ir(CFG* cfg) const
{
    std::string rval = return_expression->build_ir(cfg);

    cfg.current_bb->add_IRInstr(IRInstr::rmem, INT_64, {"%eax", rval});

    return ""; // ??
}

////////////////////////////////////////////////////////////////////////////////
// class CProgASTDeclaration : public CProgASTStatement                       //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor


////////////////////////////////////////////////////////////////////////////////
// class CProgASTAssignment : public CProgASTStatement                        //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor


////////////////////////////////////////////////////////////////////////////////
// class CProgASTAddition : public CProgASTExpression                         //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor


////////////////////////////////////////////////////////////////////////////////
// class CProgASTSubtraction : public CProgASTExpression                      //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor


////////////////////////////////////////////////////////////////////////////////
// class CProgASTMultiplication : public CProgASTExpression                   //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor


////////////////////////////////////////////////////////////////////////////////
// class CProgASTDivision : public CProgASTExpression                         //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor


////////////////////////////////////////////////////////////////////////////////
// class CProgASTModulo : public CProgASTExpression                           //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------- Constructor / Destructor
