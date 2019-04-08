#pragma once

// ------------------------------------------------------------- Project Headers
#include "antlr4-runtime.h"
#include "CProgBaseVisitor.h"

// ---------------------------------------------------------- C++ System Headers
#include <string>

class CProgCSTVisitor : public CProgBaseVisitor {
public:
    virtual antlrcpp::Any visitProgram(CProgParser::ProgramContext *ctx) override;
    virtual antlrcpp::Any visitFuncdef(CProgParser::FuncdefContext *ctx) override;
    virtual antlrcpp::Any visitStatement(CProgParser::StatementContext *ctx) override;
    virtual antlrcpp::Any visitReturn_statement(CProgParser::Return_statementContext *ctx) override;
    virtual antlrcpp::Any visitDeclaration(CProgParser::DeclarationContext *ctx) override;
    virtual antlrcpp::Any visitAssignment(CProgParser::AssignmentContext *ctx) override;
    virtual antlrcpp::Any visitExpr(CProgParser::ExprContext *ctx) override;
};
