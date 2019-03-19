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
  virtual antlrcpp::Any visitProgram(CProgParser::ProgramContext *ctx) override;
  virtual antlrcpp::Any visitFuncdef(CProgParser::FuncdefContext *ctx) override;
  virtual antlrcpp::Any visitType_name(CProgParser::Type_nameContext *ctx) override;
  virtual antlrcpp::Any visitBlock(CProgParser::BlockContext *ctx) override;
  virtual antlrcpp::Any visitStatement(CProgParser::StatementContext *ctx) override;
  virtual antlrcpp::Any visitReturn_statement(CProgParser::Return_statementContext *ctx) override;
  virtual antlrcpp::Any visitDeclaration(CProgParser::DeclarationContext *ctx) override;
  virtual antlrcpp::Any visitAssignment(CProgParser::AssignmentContext *ctx) override;
  virtual antlrcpp::Any visitExpr_sub(CProgParser::Expr_subContext *ctx) override;
  virtual antlrcpp::Any visitExpr_ass(CProgParser::Expr_assContext *ctx) override;
  virtual antlrcpp::Any visitExpr_div(CProgParser::Expr_divContext *ctx) override;
  virtual antlrcpp::Any visitExpr_add(CProgParser::Expr_addContext *ctx) override;
  virtual antlrcpp::Any visitExpr_brk(CProgParser::Expr_brkContext *ctx) override;
  virtual antlrcpp::Any visitExpr_lit(CProgParser::Expr_litContext *ctx) override;
  virtual antlrcpp::Any visitExpr_mod(CProgParser::Expr_modContext *ctx) override;
  virtual antlrcpp::Any visitExpr_mul(CProgParser::Expr_mulContext *ctx) override;
  virtual antlrcpp::Any visitExpr_id(CProgParser::Expr_idContext *ctx) override;

private:
    std::unordered_map<std::string, SymbolDef> tos;
    int tos_index = 0;
    int tmp_var_num = 0;
};
