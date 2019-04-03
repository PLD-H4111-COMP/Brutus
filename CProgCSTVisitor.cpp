// ---------------------------------------------------------- C++ System Headers
#include <iostream>
#include <string>

// ------------------------------------------------------------- Project Headers
#include "CProgCSTVisitor.h"
#include "CProgAST.h"

////////////////////////////////////////////////////////////////////////////////
// class CProgCSTVisitor                                                      //
////////////////////////////////////////////////////////////////////////////////

antlrcpp::Any CProgCSTVisitor::visitProgram(CProgParser::ProgramContext *ctx)
{
    CProgASTProgram *program = new CProgASTProgram();
    for(auto funcdef_ctx : ctx->funcdef())
    {
        program->add_funcdef(visit(funcdef_ctx).as<CProgASTFuncdef*>());
    }
    return program;
}

antlrcpp::Any CProgCSTVisitor::visitFuncdef(CProgParser::FuncdefContext *ctx)
{
    std::string identifier = ctx->IDENTIFIER()->getText();
    CProgASTFuncdef *funcdef = new CProgASTFuncdef(identifier, INT_64);
    for(auto statement_ctx : ctx->block()->statement())
    {
        funcdef->add_statement(visit(statement_ctx).as<CProgASTStatement*>());
    }
    return funcdef;
}

antlrcpp::Any CProgCSTVisitor::visitStatement(CProgParser::StatementContext *ctx)
{
    if(ctx->return_statement() != nullptr)
    {
        return static_cast<CProgASTStatement*>(visit(ctx->return_statement()).as<CProgASTReturn*>());
    }
    else if(ctx->declaration() != nullptr)
    {
        return static_cast<CProgASTStatement*>(visit(ctx->declaration()).as<CProgASTDeclaration*>());
    }
    else if(ctx->int_expr() != nullptr)
    {
        return static_cast<CProgASTStatement*>(visit(ctx->int_expr()).as<CProgASTExpression*>());
    }
    else
    {
        std::cerr << "error: empty statement currently not supported" << std::endl;
        return nullptr;
    }
}

antlrcpp::Any CProgCSTVisitor::visitReturn_statement(CProgParser::Return_statementContext *ctx)
{
    return new CProgASTReturn(visit(ctx->int_expr()).as<CProgASTExpression*>());
}

antlrcpp::Any CProgCSTVisitor::visitDeclaration(CProgParser::DeclarationContext *ctx)
{
    CProgASTDeclaration* declaration = new CProgASTDeclaration(INT_64);
    for(auto declarator_ctx : ctx->declarator())
    {
        CProgASTIdentifier* identifier = nullptr;
        CProgASTAssignment* initializer = nullptr;
        if(declarator_ctx->IDENTIFIER() != nullptr)
        {
            identifier = new CProgASTIdentifier(declarator_ctx->IDENTIFIER()->getText());
        }
        else if(declarator_ctx->assignment() != nullptr)
        {
            initializer = visit(declarator_ctx->assignment()).as<CProgASTAssignment*>();
            identifier = new CProgASTIdentifier(declarator_ctx->assignment()->IDENTIFIER()->getText());
        }
        declaration->add_declarator(new CProgASTDeclarator(identifier, initializer));
    }
    return declaration;
}

antlrcpp::Any CProgCSTVisitor::visitAssignment(CProgParser::AssignmentContext *ctx)
{
    CProgASTIdentifier* identifier = new CProgASTIdentifier(ctx->IDENTIFIER()->getText());
    CProgASTExpression* expression = visit(ctx->int_expr()).as<CProgASTExpression*>();
    return new CProgASTAssignment(identifier, expression);
}

antlrcpp::Any CProgCSTVisitor::visitInt_expr(CProgParser::Int_exprContext *ctx)
{
    if(ctx->int_terms() != nullptr)
    {
        return visit(ctx->int_terms()).as<CProgASTExpression*>();
    }
    return static_cast<CProgASTExpression*>(visit(ctx->assignment()).as<CProgASTAssignment*>());
}

antlrcpp::Any CProgCSTVisitor::visitInt_terms(CProgParser::Int_termsContext *ctx)
{
    CProgASTExpression* rexpr = visit(ctx->int_factors()).as<CProgASTExpression*>();
    for(auto rhs_int_terms_ctx : ctx->rhs_int_terms())
    {
        CProgASTExpression* expr = visit(rhs_int_terms_ctx).as<CProgASTExpression*>();
        if(rhs_int_terms_ctx->OP_ADD() != nullptr)
        {
            rexpr = new CProgASTAddition(rexpr, expr);
        }
        else if(rhs_int_terms_ctx->OP_SUB() != nullptr)
        {
            rexpr = new CProgASTSubtraction(rexpr, expr);
        }
    }
    return static_cast<CProgASTExpression*>(rexpr);
}

antlrcpp::Any CProgCSTVisitor::visitRhs_int_terms(CProgParser::Rhs_int_termsContext *ctx)
{
    return visit(ctx->int_factors()).as<CProgASTExpression*>();
}

antlrcpp::Any CProgCSTVisitor::visitInt_factors(CProgParser::Int_factorsContext *ctx)
{
    CProgASTExpression* rexpr = visit(ctx->int_signed_atom()).as<CProgASTExpression*>();
    for(auto rhs_int_factors_ctx : ctx->rhs_int_factors())
    {
        CProgASTExpression* expr = visit(rhs_int_factors_ctx).as<CProgASTExpression*>();
        if(rhs_int_factors_ctx->OP_MUL() != nullptr)
        {
            rexpr = new CProgASTMultiplication(rexpr, expr);
        }
        else if(rhs_int_factors_ctx->OP_DIV() != nullptr)
        {
            rexpr = new CProgASTDivision(rexpr, expr);
        }
        else if(rhs_int_factors_ctx->OP_MOD() != nullptr)
        {
            rexpr = new CProgASTModulo(rexpr, expr);
        }
    }
    return static_cast<CProgASTExpression*>(rexpr);
}

antlrcpp::Any CProgCSTVisitor::visitRhs_int_factors(CProgParser::Rhs_int_factorsContext *ctx)
{
    return visit(ctx->int_signed_atom()).as<CProgASTExpression*>();
}

antlrcpp::Any CProgCSTVisitor::visitInt_signed_atom(CProgParser::Int_signed_atomContext *ctx)
{
    CProgASTExpression* rexpr = nullptr;
    if(ctx->OP_SUB() != nullptr)
    {
        rexpr = new CProgASTUnaryMinus(visit(ctx->int_signed_atom()).as<CProgASTExpression*>());
    }
    else if(ctx->OP_ADD() != nullptr)
    {
        rexpr = visit(ctx->int_signed_atom()).as<CProgASTExpression*>();
    }
    else if(ctx->int_atom() != nullptr)
    {
        rexpr = visit(ctx->int_atom()).as<CProgASTExpression*>();
    }
    return static_cast<CProgASTExpression*>(rexpr);
}

antlrcpp::Any CProgCSTVisitor::visitInt_atom(CProgParser::Int_atomContext *ctx)
{
    CProgASTExpression* rexpr = nullptr;
    if(ctx->INT_LITERAL() != nullptr)
    {
        rexpr = new CProgASTIntLiteral(std::stoi(ctx->INT_LITERAL()->getText()));
    }
    else if(ctx->IDENTIFIER() != nullptr)
    {
        rexpr = new CProgASTIdentifier(ctx->IDENTIFIER()->getText());
    }
    else if(ctx->int_expr() != nullptr)
    {
        rexpr = visit(ctx->int_expr()).as<CProgASTExpression*>();
    }
    return static_cast<CProgASTExpression*>(rexpr);
}
