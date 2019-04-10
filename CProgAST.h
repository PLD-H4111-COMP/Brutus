#pragma once

// ---------------------------------------------------------- C++ System Headers
#include <iostream>
#include <string>
#include <vector>

// ------------------------------------------------------------- Project Headers
#include "IR.h"
#include "Writer.h"

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations                                                       //
////////////////////////////////////////////////////////////////////////////////

class CProgASTExpression;
class CProgASTFuncdef;
class CProgASTStatement;
class CProgASTDeclarator;
class CProgASTIdentifier;
class CProgASTIntLiteral;
class CProgASTAssignment;

////////////////////////////////////////////////////////////////////////////////
// class CProgAST                                                             //
////////////////////////////////////////////////////////////////////////////////

class CProgASTProgram {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTProgram() = default;
    CProgASTProgram(const CProgASTProgram& src) = delete;
    virtual ~CProgASTProgram();

    // ------------------------------------------------- Public Member Functions
    void add_funcdef(CProgASTFuncdef* funcdef);
    void build_ir(IR& ir) const;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTProgram& operator=(const CProgASTProgram& src) = delete;
private:
    std::vector<CProgASTFuncdef*> funcdefs;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTFuncdef                                                      //
////////////////////////////////////////////////////////////////////////////////

class CProgASTFuncdef {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTFuncdef(const std::string &id, Type type);
    CProgASTFuncdef(const CProgASTFuncdef& src) = delete;
    virtual ~CProgASTFuncdef();

    // ------------------------------------------------- Public Member Functions
    void add_statement(CProgASTStatement* statement);
    void add_arg(std::string id, Type type);
    CFG* build_ir(TableOfSymbols* global_symbols) const;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTFuncdef& operator=(const CProgASTFuncdef& src) = delete;
private:
    std::string identifier;
    Type return_type;
    std::vector<CProgASTStatement*> statements;
    std::vector<std::string> arg_names;
    std::vector<Type> arg_types;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTStatement                                                    //
////////////////////////////////////////////////////////////////////////////////

class CProgASTStatement {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTStatement() = default;
    CProgASTStatement(const CProgASTStatement& src) = delete;
    virtual ~CProgASTStatement() = default;

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const = 0;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTStatement& operator=(const CProgASTStatement& src) = delete;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTCompoundStatement : public CProgASTStatement                 //
////////////////////////////////////////////////////////////////////////////////

class CProgASTCompoundStatement : public CProgASTStatement {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTCompoundStatement() = default;
    CProgASTCompoundStatement(const CProgASTCompoundStatement& src) = delete;
    virtual ~CProgASTCompoundStatement();

    // ------------------------------------------------- Public Member Functions
    void add_statement(CProgASTStatement* statement);
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTCompoundStatement& operator=(const CProgASTCompoundStatement& src) = delete;
private:
    std::vector<const CProgASTStatement*> statements;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTReturn : public CProgASTStatement                            //
////////////////////////////////////////////////////////////////////////////////

class CProgASTReturn : public CProgASTStatement {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTReturn(CProgASTExpression* expression);
    CProgASTReturn(const CProgASTReturn& src) = delete;
    virtual ~CProgASTReturn();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTReturn& operator=(const CProgASTReturn& src) = delete;
private:
    const CProgASTExpression* return_expression;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTDeclaration : public CProgASTStatement                       //
////////////////////////////////////////////////////////////////////////////////

class CProgASTDeclaration : public CProgASTStatement {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTDeclaration(Type type);
    CProgASTDeclaration(const CProgASTDeclaration& src) = delete;
    virtual ~CProgASTDeclaration();

    // ------------------------------------------------- Public Member Functions
    void add_declarator(CProgASTDeclarator* declarator);
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTDeclaration& operator=(const CProgASTDeclaration& src) = delete;
private:
    Type type_specifier;
    std::vector<const CProgASTDeclarator*> declarators;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTDeclarator                                                   //
////////////////////////////////////////////////////////////////////////////////

class CProgASTDeclarator {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTDeclarator(CProgASTIdentifier* id, CProgASTAssignment* init);
    CProgASTDeclarator(const CProgASTDeclarator& src) = delete;
    virtual ~CProgASTDeclarator();

    // ------------------------------------------------- Public Member Functions
    void set_type(Type type);
    virtual std::string build_ir(CFG* cfg) const;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTDeclarator& operator=(const CProgASTDeclarator& src) = delete;
protected:
    const CProgASTIdentifier* identifier;
    const CProgASTAssignment* initializer;
    Type type_specifier;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTIfStatement : public CProgASTStatement                       //
////////////////////////////////////////////////////////////////////////////////

class CProgASTIfStatement : public CProgASTStatement {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTIfStatement(CProgASTExpression* condition, CProgASTStatement* if_statement, CProgASTStatement* else_statement);
    CProgASTIfStatement(const CProgASTIfStatement& src) = delete;
    virtual ~CProgASTIfStatement();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;
private:
    const CProgASTExpression* condition;
    const CProgASTStatement* if_statement;
    const CProgASTStatement* else_statement;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTWhileStatement : public CProgASTStatement                    //
////////////////////////////////////////////////////////////////////////////////

class CProgASTWhileStatement : public CProgASTStatement {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTWhileStatement(CProgASTExpression* condition, CProgASTStatement* body);
    CProgASTWhileStatement(const CProgASTWhileStatement& src) = delete;
    virtual ~CProgASTWhileStatement();

    // ------------------------------------------------- Public Member Functions
    std::string build_ir(CFG* cfg) const;
private:
    const CProgASTExpression* condition;
    const CProgASTStatement* body;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTForStatement : public CProgASTStatement                      //
////////////////////////////////////////////////////////////////////////////////

class CProgASTForStatement : public CProgASTStatement {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTForStatement(CProgASTExpression* initialization, CProgASTExpression* condition, CProgASTExpression* increment, CProgASTStatement* body);
    CProgASTForStatement(const CProgASTForStatement& src) = delete;
    virtual ~CProgASTForStatement();

    // ------------------------------------------------- Public Member Functions
    std::string build_ir(CFG* cfg) const;
private:
    const CProgASTExpression* initialization;
    const CProgASTExpression* condition;
    const CProgASTExpression* increment;
    const CProgASTStatement* body;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTExpression : public CProgASTStatement                        //
////////////////////////////////////////////////////////////////////////////////

class CProgASTExpression : public CProgASTStatement {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTExpression() = default;
    CProgASTExpression(const CProgASTExpression& src) = delete;
    virtual ~CProgASTExpression() = default;

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const = 0;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTExpression& operator=(const CProgASTExpression& src) = delete;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTAssignment : public CProgASTExpression                       //
////////////////////////////////////////////////////////////////////////////////

class CProgASTAssignment : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTAssignment(CProgASTIdentifier* identifier, CProgASTExpression* expression);
    CProgASTAssignment(const CProgASTAssignment& src) = delete;
    virtual ~CProgASTAssignment();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTAssignment& operator=(const CProgASTAssignment& src) = delete;
private:
    const CProgASTIdentifier* lhs_identifier;
    const CProgASTExpression* rhs_expression;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTPrePP : public CProgASTExpression                            //
////////////////////////////////////////////////////////////////////////////////

class CProgASTPrePP : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTPrePP(CProgASTExpression* expression);
    CProgASTPrePP(const CProgASTPrePP& src) = delete;
    virtual ~CProgASTPrePP();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTPrePP& operator=(const CProgASTPrePP& src) = delete;
private:
    const CProgASTExpression* inner_expression;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTPreMM : public CProgASTExpression                            //
////////////////////////////////////////////////////////////////////////////////

class CProgASTPreMM : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTPreMM(CProgASTExpression* expression);
    CProgASTPreMM(const CProgASTPreMM& src) = delete;
    virtual ~CProgASTPreMM();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTPreMM& operator=(const CProgASTPreMM& src) = delete;
private:
    const CProgASTExpression* inner_expression;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTPostPP : public CProgASTExpression                           //
////////////////////////////////////////////////////////////////////////////////

class CProgASTPostPP : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTPostPP(CProgASTExpression* expression);
    CProgASTPostPP(const CProgASTPostPP& src) = delete;
    virtual ~CProgASTPostPP();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTPostPP& operator=(const CProgASTPostPP& src) = delete;
private:
    const CProgASTExpression* inner_expression;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTPostMM : public CProgASTExpression                           //
////////////////////////////////////////////////////////////////////////////////

class CProgASTPostMM : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTPostMM(CProgASTExpression* expression);
    CProgASTPostMM(const CProgASTPostMM& src) = delete;
    virtual ~CProgASTPostMM();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTPostMM& operator=(const CProgASTPostMM& src) = delete;
private:
    const CProgASTExpression* inner_expression;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTBAnd : public CProgASTExpression                         //
////////////////////////////////////////////////////////////////////////////////

class CProgASTBAnd : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTBAnd(CProgASTExpression* lhs, CProgASTExpression* rhs);
    CProgASTBAnd(const CProgASTBAnd& src) = delete;
    virtual ~CProgASTBAnd();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTBAnd& operator=(const CProgASTBAnd& src) = delete;
private:
    const CProgASTExpression* lhs_operand;
    const CProgASTExpression* rhs_operand;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTBOr : public CProgASTExpression                         //
////////////////////////////////////////////////////////////////////////////////

class CProgASTBOr : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTBOr(CProgASTExpression* lhs, CProgASTExpression* rhs);
    CProgASTBOr(const CProgASTBOr& src) = delete;
    virtual ~CProgASTBOr();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTBOr& operator=(const CProgASTBOr& src) = delete;
private:
    const CProgASTExpression* lhs_operand;
    const CProgASTExpression* rhs_operand;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTBXor : public CProgASTExpression                         //
////////////////////////////////////////////////////////////////////////////////

class CProgASTBXor : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTBXor(CProgASTExpression* lhs, CProgASTExpression* rhs);
    CProgASTBXor(const CProgASTBXor& src) = delete;
    virtual ~CProgASTBXor();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTBXor& operator=(const CProgASTBXor& src) = delete;
private:
    const CProgASTExpression* lhs_operand;
    const CProgASTExpression* rhs_operand;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTBNot : public CProgASTExpression                         //
////////////////////////////////////////////////////////////////////////////////

class CProgASTBNot : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTBNot(CProgASTExpression* expression);
    CProgASTBNot(const CProgASTBNot& src) = delete;
    virtual ~CProgASTBNot();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTBNot& operator=(const CProgASTBNot& src) = delete;
private:
    const CProgASTExpression* inner_expression;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTAnd : public CProgASTExpression                              //
////////////////////////////////////////////////////////////////////////////////

class CProgASTAnd : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTAnd(CProgASTExpression* lhs, CProgASTExpression* rhs);
    CProgASTAnd(const CProgASTAnd& src) = delete;
    virtual ~CProgASTAnd();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTAnd& operator=(const CProgASTAnd& src) = delete;
private:
    const CProgASTExpression* lhs_operand;
    const CProgASTExpression* rhs_operand;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTOr : public CProgASTExpression                               //
////////////////////////////////////////////////////////////////////////////////

class CProgASTOr : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTOr(CProgASTExpression* lhs, CProgASTExpression* rhs);
    CProgASTOr(const CProgASTOr& src) = delete;
    virtual ~CProgASTOr();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTOr& operator=(const CProgASTOr& src) = delete;
private:
    const CProgASTExpression* lhs_operand;
    const CProgASTExpression* rhs_operand;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTNot : public CProgASTExpression                              //
////////////////////////////////////////////////////////////////////////////////

class CProgASTNot : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTNot(CProgASTExpression* expression);
    CProgASTNot(const CProgASTNot& src) = delete;
    virtual ~CProgASTNot();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTNot& operator=(const CProgASTNot& src) = delete;
private:
    const CProgASTExpression* inner_expression;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTLessThan : public CProgASTExpression                         //
////////////////////////////////////////////////////////////////////////////////

class CProgASTLessThan : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTLessThan(CProgASTExpression* lhs, CProgASTExpression* rhs);
    CProgASTLessThan(const CProgASTLessThan& src) = delete;
    virtual ~CProgASTLessThan();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTLessThan& operator=(const CProgASTLessThan& src) = delete;
private:
    const CProgASTExpression* lhs_operand;
    const CProgASTExpression* rhs_operand;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTLessThanOrEqual : public CProgASTExpression                         //
////////////////////////////////////////////////////////////////////////////////

class CProgASTLessThanOrEqual : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTLessThanOrEqual(CProgASTExpression* lhs, CProgASTExpression* rhs);
    CProgASTLessThanOrEqual(const CProgASTLessThanOrEqual& src) = delete;
    virtual ~CProgASTLessThanOrEqual();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTLessThanOrEqual& operator=(const CProgASTLessThanOrEqual& src) = delete;
private:
    const CProgASTExpression* lhs_operand;
    const CProgASTExpression* rhs_operand;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTGreaterThan : public CProgASTExpression                      //
////////////////////////////////////////////////////////////////////////////////

class CProgASTGreaterThan : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTGreaterThan(CProgASTExpression* lhs, CProgASTExpression* rhs);
    CProgASTGreaterThan(const CProgASTGreaterThan& src) = delete;
    virtual ~CProgASTGreaterThan();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTGreaterThan& operator=(const CProgASTGreaterThan& src) = delete;
private:
    const CProgASTExpression* lhs_operand;
    const CProgASTExpression* rhs_operand;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTGreaterThanOrEqual : public CProgASTExpression               //
////////////////////////////////////////////////////////////////////////////////

class CProgASTGreaterThanOrEqual : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTGreaterThanOrEqual(CProgASTExpression* lhs, CProgASTExpression* rhs);
    CProgASTGreaterThanOrEqual(const CProgASTGreaterThanOrEqual& src) = delete;
    virtual ~CProgASTGreaterThanOrEqual();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTGreaterThanOrEqual& operator=(const CProgASTGreaterThanOrEqual& src) = delete;
private:
    const CProgASTExpression* lhs_operand;
    const CProgASTExpression* rhs_operand;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTGreaterThanOrEqual : public CProgASTExpression               //
////////////////////////////////////////////////////////////////////////////////

class CProgASTEqual : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTEqual(CProgASTExpression* lhs, CProgASTExpression* rhs);
    CProgASTEqual(const CProgASTEqual& src) = delete;
    virtual ~CProgASTEqual();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTEqual& operator=(const CProgASTEqual& src) = delete;
private:
    const CProgASTExpression* lhs_operand;
    const CProgASTExpression* rhs_operand;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTNotEqual : public CProgASTExpression               //
////////////////////////////////////////////////////////////////////////////////

class CProgASTNotEqual : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTNotEqual(CProgASTExpression* lhs, CProgASTExpression* rhs);
    CProgASTNotEqual(const CProgASTNotEqual& src) = delete;
    virtual ~CProgASTNotEqual();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTNotEqual& operator=(const CProgASTNotEqual& src) = delete;
private:
    const CProgASTExpression* lhs_operand;
    const CProgASTExpression* rhs_operand;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTAddition : public CProgASTExpression                         //
////////////////////////////////////////////////////////////////////////////////

class CProgASTAddition : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTAddition(CProgASTExpression* lhs, CProgASTExpression* rhs);
    CProgASTAddition(const CProgASTAddition& src) = delete;
    virtual ~CProgASTAddition();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTAddition& operator=(const CProgASTAddition& src) = delete;
private:
    const CProgASTExpression* lhs_operand;
    const CProgASTExpression* rhs_operand;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTSubtraction : public CProgASTExpression                      //
////////////////////////////////////////////////////////////////////////////////

class CProgASTSubtraction : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTSubtraction(CProgASTExpression* lhs, CProgASTExpression* rhs);
    CProgASTSubtraction(const CProgASTSubtraction& src) = delete;
    virtual ~CProgASTSubtraction();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTSubtraction& operator=(const CProgASTSubtraction& src) = delete;
private:
    const CProgASTExpression* lhs_operand;
    const CProgASTExpression* rhs_operand;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTMultiplication : public CProgASTExpression                   //
////////////////////////////////////////////////////////////////////////////////

class CProgASTMultiplication : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTMultiplication(CProgASTExpression* lhs, CProgASTExpression* rhs);
    CProgASTMultiplication(const CProgASTMultiplication& src) = delete;
    virtual ~CProgASTMultiplication();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTMultiplication& operator=(const CProgASTMultiplication& src) = delete;
private:
    const CProgASTExpression* lhs_operand;
    const CProgASTExpression* rhs_operand;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTDivision : public CProgASTExpression                         //
////////////////////////////////////////////////////////////////////////////////

class CProgASTDivision : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTDivision(CProgASTExpression* lhs, CProgASTExpression* rhs);
    CProgASTDivision(const CProgASTDivision& src) = delete;
    virtual ~CProgASTDivision();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTDivision& operator=(const CProgASTDivision& src) = delete;
private:
    const CProgASTExpression* lhs_operand;
    const CProgASTExpression* rhs_operand;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTModulo : public CProgASTExpression                           //
////////////////////////////////////////////////////////////////////////////////

class CProgASTModulo : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTModulo(CProgASTExpression* lhs, CProgASTExpression* rhs);
    CProgASTModulo(const CProgASTModulo& src) = delete;
    virtual ~CProgASTModulo();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTModulo& operator=(const CProgASTModulo& src) = delete;
private:
    const CProgASTExpression* lhs_operand;
    const CProgASTExpression* rhs_operand;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTUnaryMinus : public CProgASTExpression                       //
////////////////////////////////////////////////////////////////////////////////

class CProgASTUnaryMinus : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTUnaryMinus(CProgASTExpression* expression);
    CProgASTUnaryMinus(const CProgASTUnaryMinus& src) = delete;
    virtual ~CProgASTUnaryMinus();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTUnaryMinus& operator=(const CProgASTUnaryMinus& src) = delete;
private:
    const CProgASTExpression* inner_expression;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTFunccall : public CProgASTExpression                         //
////////////////////////////////////////////////////////////////////////////////

class CProgASTFunccall : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTFunccall(CProgASTIdentifier* identifier);
    CProgASTFunccall(const CProgASTFunccall& src) = delete;
    virtual ~CProgASTFunccall();

    // ------------------------------------------------- Public Member Functions
    void add_arg(CProgASTExpression* arg);
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTFunccall& operator=(const CProgASTFunccall& src) = delete;
private:
    const CProgASTIdentifier* func_name;
    std::vector<CProgASTExpression*> args;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTIntLiteral : public CProgASTExpression                       //
////////////////////////////////////////////////////////////////////////////////

class CProgASTIntLiteral : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTIntLiteral(int64_t val);
    CProgASTIntLiteral(const CProgASTIntLiteral& src) = delete;
    virtual ~CProgASTIntLiteral() = default;

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTIntLiteral& operator=(const CProgASTIntLiteral& src) = delete;
private:
    const int64_t value;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTCharLiteral : public CProgASTExpression                      //
////////////////////////////////////////////////////////////////////////////////

class CProgASTCharLiteral : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTCharLiteral(const std::string &val);
    CProgASTCharLiteral(const CProgASTCharLiteral& src) = delete;
    virtual ~CProgASTCharLiteral() = default;

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTCharLiteral& operator=(const CProgASTCharLiteral& src) = delete;
private:
    const std::string value;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTIdentifier : public CProgASTExpression                       //
////////////////////////////////////////////////////////////////////////////////

class CProgASTIdentifier : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTIdentifier(const std::string &identifier);
    CProgASTIdentifier(const CProgASTIdentifier& src) = delete;
    virtual ~CProgASTIdentifier() = default;

    // ------------------------------------------------- Public Member Functions
    std::string getText() const;
    virtual std::string build_ir(CFG* cfg) const override;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTIdentifier& operator=(const CProgASTIdentifier& src) = delete;
private:
    const std::string name;
};
