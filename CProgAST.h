#pragma once

// ---------------------------------------------------------- C++ System Headers
#include <iostream>
#include <string>
#include <vector>

#include "IR.h"

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
// class CProgASTNode                                                         //
////////////////////////////////////////////////////////////////////////////////

class CProgASTNode {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTNode() = default;
    CProgASTNode(const CProgASTNode& src) = delete;
    virtual ~CProgASTNode() = default;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTNode& operator=(const CProgASTNode& src) = delete;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgAST : public CProgASTNode                                       //
////////////////////////////////////////////////////////////////////////////////

class CProgASTProgram : public CProgASTNode {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTProgram() = default;
    CProgASTProgram(const CProgASTProgram& src) = delete;
    virtual ~CProgASTProgram();

    // ------------------------------------------------- Public Member Functions
    void add_funcdef(CProgASTFuncdef* funcdef);
    std::vector<CFG*> build_ir() const;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTProgram& operator=(const CProgASTProgram& src) = delete;
private:
    std::vector<CProgASTFuncdef*> funcdefs;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTFuncdef : public CProgASTNode                                //
////////////////////////////////////////////////////////////////////////////////

class CProgASTFuncdef : public CProgASTNode {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTFuncdef(std::string id, Type type);
    CProgASTFuncdef(const CProgASTFuncdef& src) = delete;
    virtual ~CProgASTFuncdef();

    // ------------------------------------------------- Public Member Functions
    void add_statement(CProgASTStatement* statement);
    CFG* build_ir() const;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTFuncdef& operator=(const CProgASTFuncdef& src) = delete;
private:
    std::string identifier;
    Type return_type;
    std::vector<CProgASTStatement*> statements;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTStatement : public CProgASTNode                              //
////////////////////////////////////////////////////////////////////////////////

class CProgASTStatement : public CProgASTNode {
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
// class CProgASTReturn : public CProgASTStatement                            //
////////////////////////////////////////////////////////////////////////////////

class CProgASTReturn : public CProgASTStatement {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTReturn(CProgASTExpression* expression);
    CProgASTReturn(const CProgASTReturn& src) = delete;
    virtual ~CProgASTReturn();

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const;

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
    virtual std::string build_ir(CFG* cfg) const;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTDeclaration& operator=(const CProgASTDeclaration& src) = delete;
private:
    Type type_specifier;
    std::vector<const CProgASTDeclarator*> declarators;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTDeclarator : public CProgASTNode                             //
////////////////////////////////////////////////////////////////////////////////

class CProgASTDeclarator : public CProgASTNode {
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
    virtual std::string build_ir(CFG* cfg) const;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTAssignment& operator=(const CProgASTAssignment& src) = delete;
private:
    const CProgASTIdentifier* lhs_identifier;
    const CProgASTExpression* rhs_expression;
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
    virtual std::string build_ir(CFG* cfg) const;

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
    virtual std::string build_ir(CFG* cfg) const;

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
    virtual std::string build_ir(CFG* cfg) const;

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
    virtual std::string build_ir(CFG* cfg) const;

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
    virtual std::string build_ir(CFG* cfg) const;

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
    virtual std::string build_ir(CFG* cfg) const;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTUnaryMinus& operator=(const CProgASTUnaryMinus& src) = delete;
private:
    const CProgASTExpression* inner_expression;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTIntLiteral : public CProgASTExpression                       //
////////////////////////////////////////////////////////////////////////////////

class CProgASTIntLiteral : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTIntLiteral(int val);
    CProgASTIntLiteral(const CProgASTIntLiteral& src) = delete;
    virtual ~CProgASTIntLiteral() = default;

    // ------------------------------------------------- Public Member Functions
    virtual std::string build_ir(CFG* cfg) const;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTIntLiteral& operator=(const CProgASTIntLiteral& src) = delete;
private:
    const int value;
};

////////////////////////////////////////////////////////////////////////////////
// class CProgASTIdentifier : public CProgASTExpression                       //
////////////////////////////////////////////////////////////////////////////////

class CProgASTIdentifier : public CProgASTExpression {
public:
    // ------------------------------------------------ Constructor / Destructor
    CProgASTIdentifier(std::string identifier);
    CProgASTIdentifier(const CProgASTIdentifier& src) = delete;
    virtual ~CProgASTIdentifier() = default;

    // ------------------------------------------------- Public Member Functions
    std::string getText() const;
    virtual std::string build_ir(CFG* cfg) const;

    // ---------------------------------------------------- Overloaded Operators
    CProgASTIdentifier& operator=(const CProgASTIdentifier& src) = delete;
private:
    const std::string name;
};
