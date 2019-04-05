// ------------------------------------------------------------- Project Headers
#include "CProgCSTVisitor.h"
#include "CProgAST.h"
#include "Writer.h"

// ---------------------------------------------------------- C++ System Headers
#include <iostream>
#include <string>

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
    CProgASTFuncdef *funcdef = new CProgASTFuncdef(identifier, Type::INT_64);
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
    else if(ctx->expr() != nullptr)
    {
        return static_cast<CProgASTStatement*>(visit(ctx->expr()).as<CProgASTExpression*>());
    }
    else
    {
        Writer::error() << "empty statement currently not supported" << std::endl;
        return nullptr;
    }
}

antlrcpp::Any CProgCSTVisitor::visitReturn_statement(CProgParser::Return_statementContext *ctx)
{
    return new CProgASTReturn(visit(ctx->expr()).as<CProgASTExpression*>());
}

antlrcpp::Any CProgCSTVisitor::visitDeclaration(CProgParser::DeclarationContext *ctx)
{
    Type type;
    if (ctx->type_name()->CHAR_TYPE_NAME())
        type = Type::CHAR;
    else if (ctx->type_name()->INT_16_TYPE_NAME())
        type = Type::INT_16;
    else if (ctx->type_name()->INT_32_TYPE_NAME())
        type = Type::INT_32;
    else if (ctx->type_name()->INT_64_TYPE_NAME() || ctx->type_name()->INT_TYPE_NAME())
        type = Type::INT_64;
    else if (ctx->type_name()->VOID_TYPE_NAME())
    {
        Writer::error() << "can't declare a void variable" << std::endl;
        return nullptr;
    }
    else if (ctx->type_name()->IDENTIFIER())
    {
        Writer::error() << "unknown type name '" << ctx->type_name()->IDENTIFIER()->getText() << "'" << std::endl;
        return nullptr;
    }

    CProgASTDeclaration* declaration = new CProgASTDeclaration(type);
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
    CProgASTExpression* expression = visit(ctx->expr()).as<CProgASTExpression*>();
    return new CProgASTAssignment(identifier, expression);
}

antlrcpp::Any CProgCSTVisitor::visitExpr(CProgParser::ExprContext *ctx)
{
    return static_cast<CProgASTExpression*>(visit(ctx->asgn_expr()).as<CProgASTExpression*>());
}

antlrcpp::Any CProgCSTVisitor::visitAsgn_expr(CProgParser::Asgn_exprContext *ctx)
{
    CProgASTExpression* rexpr = visit(ctx->or_expr()).as<CProgASTExpression*>();
    for(auto asgn_lhs_ctx : ctx->asgn_lhs())
    {
        if(asgn_lhs_ctx->asgn_op()->OP_ASGN() != nullptr)
        {
            CProgASTIdentifier *identifier = new CProgASTIdentifier(asgn_lhs_ctx->IDENTIFIER()->getText());
            rexpr = new CProgASTAssignment(identifier, rexpr);
        }
    }
    return static_cast<CProgASTExpression*>(rexpr);
}

antlrcpp::Any CProgCSTVisitor::visitOr_expr(CProgParser::Or_exprContext *ctx)
{
    return static_cast<CProgASTExpression*>(visit(ctx->and_expr()).as<CProgASTExpression*>());
}

antlrcpp::Any CProgCSTVisitor::visitAnd_expr(CProgParser::And_exprContext *ctx)
{
    return static_cast<CProgASTExpression*>(visit(ctx->bor_expr()).as<CProgASTExpression*>());
}

antlrcpp::Any CProgCSTVisitor::visitBor_expr(CProgParser::Bor_exprContext *ctx)
{
    return static_cast<CProgASTExpression*>(visit(ctx->xor_expr()).as<CProgASTExpression*>());
}

antlrcpp::Any CProgCSTVisitor::visitXor_expr(CProgParser::Xor_exprContext *ctx)
{
    return static_cast<CProgASTExpression*>(visit(ctx->band_expr()).as<CProgASTExpression*>());
}

antlrcpp::Any CProgCSTVisitor::visitBand_expr(CProgParser::Band_exprContext *ctx)
{
    return static_cast<CProgASTExpression*>(visit(ctx->eq_expr()).as<CProgASTExpression*>());
}

antlrcpp::Any CProgCSTVisitor::visitEq_expr(CProgParser::Eq_exprContext *ctx)
{
    return static_cast<CProgASTExpression*>(visit(ctx->rel_expr()).as<CProgASTExpression*>());
}

antlrcpp::Any CProgCSTVisitor::visitRel_expr(CProgParser::Rel_exprContext *ctx)
{
    return static_cast<CProgASTExpression*>(visit(ctx->add_expr()).as<CProgASTExpression*>());
}

antlrcpp::Any CProgCSTVisitor::visitAdd_expr(CProgParser::Add_exprContext *ctx)
{
    CProgASTExpression* rexpr = visit(ctx->mult_expr()).as<CProgASTExpression*>();
    for(auto add_rhs_ctx : ctx->add_rhs())
    {
        CProgASTExpression* expr = visit(add_rhs_ctx->mult_expr()).as<CProgASTExpression*>();
        if(add_rhs_ctx->add_op()->OP_PLUS() != nullptr)
        {
            rexpr = new CProgASTAddition(rexpr, expr);
        }
        else if(add_rhs_ctx->add_op()->OP_MINUS() != nullptr)
        {
            rexpr = new CProgASTSubtraction(rexpr, expr);
        }
    }
    return static_cast<CProgASTExpression*>(rexpr);
}

antlrcpp::Any CProgCSTVisitor::visitMult_expr(CProgParser::Mult_exprContext *ctx)
{
    CProgASTExpression* rexpr = visit(ctx->unary_expr()).as<CProgASTExpression*>();
    for(auto mult_rhs_ctx : ctx->mult_rhs())
    {
        CProgASTExpression* expr = visit(mult_rhs_ctx->unary_expr()).as<CProgASTExpression*>();
        if(mult_rhs_ctx->mult_op()->OP_MUL() != nullptr)
        {
            rexpr = new CProgASTMultiplication(rexpr, expr);
        }
        else if(mult_rhs_ctx->mult_op()->OP_DIV() != nullptr)
        {
            rexpr = new CProgASTDivision(rexpr, expr);
        }
        else if(mult_rhs_ctx->mult_op()->OP_MOD() != nullptr)
        {
            rexpr = new CProgASTModulo(rexpr, expr);
        }
    }
    return static_cast<CProgASTExpression*>(rexpr);
}

antlrcpp::Any CProgCSTVisitor::visitUnary_expr(CProgParser::Unary_exprContext *ctx)
{
    CProgASTExpression* rexpr = visit(ctx->postfix_expr()).as<CProgASTExpression*>();
    for(auto unary_lhs_ctx : ctx->unary_lhs()) // TODO : reverse order
    {
        if(unary_lhs_ctx->unary_op()->OP_PLUS() != nullptr)
        {
            // NOP
        }
        else if(unary_lhs_ctx->unary_op()->OP_MINUS() != nullptr)
        {
            rexpr = new CProgASTUnaryMinus(rexpr);
        }
        else if(unary_lhs_ctx->unary_op()->OP_PP() != nullptr)
        {
            // TODO
        }
        else if(unary_lhs_ctx->unary_op()->OP_MM() != nullptr)
        {
            // TODO
        }
        else if(unary_lhs_ctx->unary_op()->OP_NOT() != nullptr)
        {
            // TODO
        }
        else if(unary_lhs_ctx->unary_op()->OP_BNOT() != nullptr)
        {
            // TODO
        }
    }
    return static_cast<CProgASTExpression*>(rexpr);
}

antlrcpp::Any CProgCSTVisitor::visitPostfix_expr(CProgParser::Postfix_exprContext *ctx)
{
    return static_cast<CProgASTExpression*>(visit(ctx->atom_expr()).as<CProgASTExpression*>());
}

antlrcpp::Any CProgCSTVisitor::visitAtom_expr(CProgParser::Atom_exprContext *ctx)
{
    CProgASTExpression* rexpr = nullptr;
    if(ctx->INT_LITERAL() != nullptr)
    {
        rexpr = new CProgASTIntLiteral(std::stoi(ctx->INT_LITERAL()->getText()));
    }
    else if(ctx->CHAR_LITERAL() != nullptr)
    {
        std::string literal = ctx->CHAR_LITERAL()->getText();
        rexpr = new CProgASTCharLiteral(literal.substr(1, literal.size()-2));
    }
    else if(ctx->IDENTIFIER() != nullptr)
    {
        rexpr = new CProgASTIdentifier(ctx->IDENTIFIER()->getText());
    }
    else if(ctx->expr() != nullptr)
    {
        rexpr = visit(ctx->expr()).as<CProgASTExpression*>();
    }
    return static_cast<CProgASTExpression*>(rexpr);
}
