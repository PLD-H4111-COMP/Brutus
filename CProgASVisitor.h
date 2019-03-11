#pragma once

#include "antlr4-runtime.h"
#include "CProgBaseVisitor.h"
#include <string>
#include <unordered_map>

enum class SymbolType { INTEGER };

struct SymbolDef
{
    SymbolType type;
    int index;
    bool initialized;
    bool used;
};

class CProgASVisitor : public CProgBaseVisitor {
public:
    virtual antlrcpp::Any visitPreproc(CProgParser::PreprocContext *ctx) override;
    virtual antlrcpp::Any visitProg(CProgParser::ProgContext *ctx) override;
    virtual antlrcpp::Any visitFuncdef(CProgParser::FuncdefContext *ctx) override;
    virtual antlrcpp::Any visitType(CProgParser::TypeContext *ctx) override;
    virtual antlrcpp::Any visitIdentifier(CProgParser::IdentifierContext *ctx) override;
    virtual antlrcpp::Any visitBlock(CProgParser::BlockContext *ctx) override;
    virtual antlrcpp::Any visitStatement(CProgParser::StatementContext *ctx) override;
    virtual antlrcpp::Any visitReturn_statement(CProgParser::Return_statementContext *ctx) override;
    virtual antlrcpp::Any visitDeclaration(CProgParser::DeclarationContext *ctx) override;
    virtual antlrcpp::Any visitAssignment(CProgParser::AssignmentContext *ctx) override;
    
private:
    std::unordered_map<std::string, SymbolDef> tos;
    int tos_index = 0;
};

