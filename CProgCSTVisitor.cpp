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
        antlrcpp::Any statement = visit(statement_ctx);
        try {
            funcdef->add_statement(statement.as<CProgASTReturn*>());
        } catch (std::bad_cast e) {}
        try {
            funcdef->add_statement(statement.as<CProgASTDeclaration*>());
        } catch (std::bad_cast e) {}
        try {
            funcdef->add_statement(statement.as<CProgASTExpression*>());
        } catch (std::bad_cast e) {}
    }
    return funcdef;
}

antlrcpp::Any CProgCSTVisitor::visitStatement(CProgParser::StatementContext *ctx)
{
    if(ctx->return_statement() != nullptr)
    {
        return visit(ctx->return_statement()).as<CProgASTReturn*>();
    }
    else if(ctx->declaration() != nullptr)
    {
        return visit(ctx->declaration()).as<CProgASTDeclaration*>();
    }
    else if(ctx->int_expr() != nullptr)
    {
        return visit(ctx->int_expr()).as<CProgASTExpression*>();
    }
    else
    {
        std::cerr << "error: empty statement currently not supported" << std::endl;
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
    return visit(ctx->assignment()).as<CProgASTExpression*>();
}

antlrcpp::Any CProgCSTVisitor::visitInt_terms(CProgParser::Int_termsContext *ctx)
{
    CProgASTExpression* rexpr = visit(ctx->int_factors()).as<CProgASTExpression*>();
    for(auto rhs_int_terms_ctx : ctx->rhs_int_terms())
    {
        CProgASTExpression* expr = visit(rhs_int_terms_ctx).as<CProgASTExpression*>();
        if(rhs_int_terms_ctx->OP_ADD() != nullptr)
        {
            rexpr = dynamic_cast<CProgASTExpression*>(new CProgASTAddition(rexpr, expr));
        }
        else if(rhs_int_terms_ctx->OP_SUB() != nullptr)
        {
            rexpr = dynamic_cast<CProgASTExpression*>(new CProgASTSubtraction(rexpr, expr));
        }
    }
    return rexpr;
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
        CProgASTExpression* expr = (CProgASTExpression*) visit(rhs_int_factors_ctx);
        if(rhs_int_factors_ctx->OP_MUL() != nullptr)
        {
            rexpr = dynamic_cast<CProgASTExpression*>(new CProgASTMultiplication(rexpr, expr));
        }
        else if(rhs_int_factors_ctx->OP_DIV() != nullptr)
        {
            rexpr = dynamic_cast<CProgASTExpression*>(new CProgASTDivision(rexpr, expr));
        }
        else if(rhs_int_factors_ctx->OP_MOD() != nullptr)
        {
            rexpr = dynamic_cast<CProgASTExpression*>(new CProgASTModulo(rexpr, expr));
        }
    }
    return rexpr;
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
        rexpr = dynamic_cast<CProgASTExpression*>(new CProgASTUnaryMinus(visit(ctx->int_signed_atom()).as<CProgASTExpression*>()));
    }
    else if(ctx->OP_ADD() != nullptr)
    {
        rexpr = visit(ctx->int_signed_atom()).as<CProgASTExpression*>();
    }
    else if(ctx->int_atom() != nullptr)
    {
        rexpr = visit(ctx->int_atom()).as<CProgASTExpression*>();
    }
    return rexpr;
}

antlrcpp::Any CProgCSTVisitor::visitInt_atom(CProgParser::Int_atomContext *ctx)
{
    CProgASTExpression* rexpr = nullptr;
    if(ctx->INT_LITERAL() != nullptr)
    {
        rexpr = dynamic_cast<CProgASTExpression*>(new CProgASTIntLiteral(std::stoi(ctx->INT_LITERAL()->getText())));
    }
    else if(ctx->IDENTIFIER() != nullptr)
    {
        rexpr = dynamic_cast<CProgASTExpression*>(new CProgASTIdentifier(ctx->IDENTIFIER()->getText()));
    }
    else if(ctx->int_expr() != nullptr)
    {
        rexpr = visit(ctx->int_expr()).as<CProgASTExpression*>();
    }
    return rexpr;
}
