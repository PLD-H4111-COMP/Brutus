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

class TableOfSymbols {
public:
    TableOfSymbols() : index(0), tmp_res_num(0) {}

    std::string add_tmp_result()
    {
        std::string name = "!tmp" + std::to_string(tmp_res_num++);
        add_variable(name);
        return name;
    }

    void add_variable(std::string identifier)
    {
        index -= 4; // size of the identifier's type
        symbols[identifier] = {SymbolType::INTEGER, index, false, false};
    }

    bool declared(std::string identifier)
    {
        return symbols.find(identifier) != symbols.end();
    }

    SymbolDef & operator[](const std::string & name)
    {
        return symbols[name];
    }

protected:
    std::unordered_map<std::string, SymbolDef> symbols;
    int index;
    int tmp_res_num;
};

class CProgCSTVisitor : public CProgBaseVisitor {
public:
    virtual antlrcpp::Any visitProgram(CProgParser::ProgramContext *ctx) override;
    virtual antlrcpp::Any visitFuncdef(CProgParser::FuncdefContext *ctx) override;
    virtual antlrcpp::Any visitStatement(CProgParser::StatementContext *ctx) override;
    virtual antlrcpp::Any visitReturn_statement(CProgParser::Return_statementContext *ctx) override;
    virtual antlrcpp::Any visitDeclaration(CProgParser::DeclarationContext *ctx) override;
    virtual antlrcpp::Any visitAssignment(CProgParser::AssignmentContext *ctx) override;
    virtual antlrcpp::Any visitInt_expr(CProgParser::Int_exprContext *ctx) override;
    virtual antlrcpp::Any visitInt_terms(CProgParser::Int_termsContext *ctx) override;
    virtual antlrcpp::Any visitRhs_int_terms(CProgParser::Rhs_int_termsContext *ctx) override;
    virtual antlrcpp::Any visitInt_factors(CProgParser::Int_factorsContext *ctx) override;
    virtual antlrcpp::Any visitRhs_int_factors(CProgParser::Rhs_int_factorsContext *ctx) override;
    virtual antlrcpp::Any visitInt_signed_atom(CProgParser::Int_signed_atomContext *ctx) override;
    virtual antlrcpp::Any visitInt_atom(CProgParser::Int_atomContext *ctx) override;

private:
    TableOfSymbols tos;
};
