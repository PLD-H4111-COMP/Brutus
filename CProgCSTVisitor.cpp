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
    CProgAST *root = new CProgAST();
    for(auto funcdef_ctx : ctx->funcdef())
    {
        root->add_funcdef((CProgASTFuncdef*) visit(funcdef_ctx));
    }
    return root;
}

antlrcpp::Any CProgCSTVisitor::visitFuncdef(CProgParser::FuncdefContext *ctx)
{
    std::string identifier = ctx->IDENTIFIER->getText();
    CProgASTFuncdef *funcdef = new CProgASTFuncdef(identifier, INT_64);
    for(auto statement_ctx : ctx->block()->statement())
    {
        funcdef->add_statement((CProgASTStatement*) visit(statement_ctx));
    }
    return funcdef;
}

antlrcpp::Any CProgCSTVisitor::visitReturn_statement(CProgParser::Return_statementContext *ctx)
{
    return new CProgASTReturn((CProgASTExpression*) visit(ctx->int_expr()));
}

antlrcpp::Any CProgCSTVisitor::visitDeclaration(CProgParser::DeclarationContext *ctx)
{
    CProgASTDeclaration* declaration = new CProgASTDeclaration(INT_64);
    for(auto declarator_ctx : ctx->declarator())
    {
        if(declarator_ctx->IDENTIFIER() != nullptr)
        {
            declaration->add_declarator(new CProgASTIdentifier(declarator_ctx->IDENTIFIER()->getText()));
        }
        else if(declarator_ctx->assignment() != nullptr)
        {
            declaration->add_declarator((CProgASTAssignment*) visit(declarator_ctx->assignment()));
        }
    }
    return declaration;
}

antlrcpp::Any CProgCSTVisitor::visitAssignment(CProgParser::AssignmentContext *ctx)
{
    CProgASTIdentifier* identifier = new CProgASTIdentifier(ctx->IDENTIFIER()->getText());
    CProgASTExpression* expression = (CProgASTExpression*) visit(ctx->int_expr());
    return new CProgASTAssignment(identifier, expression);
}

antlrcpp::Any CProgCSTVisitor::visitInt_expr(CProgParser::Int_exprContext *ctx)
{
    if(ctx->int_terms() != nullptr)
    {
        return (CProgASTExpression*) visit(ctx->int_terms());
    }
    return (CProgASTAssignment*) visit(ctx->assignment());
}

antlrcpp::Any CProgCSTVisitor::visitInt_terms(CProgParser::Int_termsContext *ctx)
{
    CProgASTExpression* root = (CProgASTExpression*) visit(ctx->int_factors());
    for(auto rhs_int_terms_ctx : ctx->rhs_int_terms())
    {
        CProgASTExpression* expr = (CProgASTExpression*) visit(rhs_int_terms_ctx);
        if(rhs_int_terms_ctx->OP_ADD() != nullptr)
        {
            root = new CProgASTAddition(root, expr);
        }
        else if(rhs_int_terms_ctx->OP_SUB() != nullptr)
        {
            root = new CProgASTSubtraction(root, expr);
        }
    }
    return root;
}

antlrcpp::Any CProgCSTVisitor::visitRhs_int_terms(CProgParser::Rhs_int_termsContext *ctx)
{
    return (CProgASTExpression*) visit(ctx->int_factors());
}

antlrcpp::Any CProgCSTVisitor::visitInt_factors(CProgParser::Int_factorsContext *ctx)
{
    CProgASTExpression* root = (CProgASTExpression*) visit(ctx->int_signed_atom());
    for(auto rhs_int_factors_ctx : ctx->rhs_int_factors())
    {
        CProgASTExpression* expr = (CProgASTExpression*) visit(rhs_int_factors_ctx);
        if(rhs_int_factors_ctx->OP_MUL() != nullptr)
        {
            root = new CProgASTMultiplication(root, expr);
        }
        else if(rhs_int_factors_ctx->OP_DIV() != nullptr)
        {
            root = new CProgASTDivision(root, expr);
        }
        else if(rhs_int_factors_ctx->OP_MOD() != nullptr)
        {
            root = new CProgASTModulo(root, expr);
        }
    }
    return root;
}

antlrcpp::Any CProgCSTVisitor::visitRhs_int_factors(CProgParser::Rhs_int_factorsContext *ctx)
{
    return (CProgASTExpression*) visit(ctx->int_signed_atom());
}

antlrcpp::Any CProgCSTVisitor::visitInt_signed_atom(CProgParser::Int_signed_atomContext *ctx)
{
    if(ctx->OP_SUB() != nullptr)
    {
        return new CProgASTUnaryMinus(visit(ctx->int_signed_atom()));
    }
    if(ctx->OP_ADD() != nullptr)
    {
        return (CProgASTExpression*) visit(ctx->int_signed_atom());
    }
    return (CProgASTExpression*) visit(ctx->int_atom());
}

antlrcpp::Any CProgCSTVisitor::visitInt_atom(CProgParser::Int_atomContext *ctx)
{
    if(ctx->INT_LITTERAL() != nullptr)
    {
        return new CProgASTIntLiteral(std::atoi(ctx->INT_LITTERAL()->getText()));
    }
    if(ctx->IDENTIFIER() != nullptr)
    {
        return new CProgASTIdentifier(ctx->IDENTIFIER()->getText());

    }
    return (CProgASTExpression*) visit(ctx->int_expr());
}
